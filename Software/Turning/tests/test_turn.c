#include "turn.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t sensors[SENSOR_COUNT];
static unsigned reads[SENSOR_COUNT];
static unsigned motorWrites;
static unsigned updates;
static int16_t motorLeft;
static int16_t motorRight;
static int16_t encoderLeft;
static int16_t encoderRight;
static unsigned encoderReads;

/* Sensor, motor and encoder connections are deterministic host mocks. */
uint8_t ReadSensor(SensorId sensor)
{
    assert(sensor >= SENSOR_FL && sensor < SENSOR_COUNT);
    ++reads[sensor];
    return sensors[sensor];
}

void SetMotorSpeed(int16_t left, int16_t right)
{
    assert(left >= -100 && left <= 100);
    assert(right >= -100 && right <= 100);
    ++motorWrites;
    motorLeft = left;
    motorRight = right;
}

void ReadWheelEncoderCounts(int16_t *left, int16_t *right)
{
    ++encoderReads;
    *left = encoderLeft;
    *right = encoderRight;
}

static int16_t WrapCount(int32_t value)
{
    while (value > 32767L) value -= 65536L;
    while (value < -32768L) value += 65536L;
    return (int16_t)value;
}

static void Advance(int32_t left, int32_t right)
{
    encoderLeft = WrapCount((int32_t)encoderLeft +
                           left * TURN_LEFT_ENCODER_FORWARD_SIGN);
    encoderRight = WrapCount((int32_t)encoderRight +
                            right * TURN_RIGHT_ENCODER_FORWARD_SIGN);
}

static TurnState Tick(void)
{
    TurnState result;
    ++updates;
    result = TurnUpdate();
    assert(TurnGetState() == result);
    return result;
}

static void ExpectTicks(unsigned count, TurnState expected)
{
    unsigned i;
    for (i = 0; i < count; ++i) {
        assert(Tick() == expected);
    }
}

static void Middle(uint8_t left, uint8_t right)
{
    sensors[SENSOR_FM_L] = left;
    sensors[SENSOR_FM_R] = right;
}

static void Begin(TurnDirection direction)
{
    TurnReset();
    assert(TurnGetState() == TURN_IDLE);
    memset(sensors, 0, sizeof sensors);
    memset(reads, 0, sizeof reads);
    updates = 0;
    /* Nonzero baselines near wrap. Alternate build reverses encoder polarity,
     * so these tests cross both signed-counter boundaries on forward travel.
     */
    encoderLeft = TURN_LEFT_ENCODER_FORWARD_SIGN > 0 ? 32760 : -32760;
    encoderRight = TURN_RIGHT_ENCODER_FORWARD_SIGN > 0 ? 32760 : -32760;
    Middle(1, 1);
    assert(TurnStart(direction));
    assert(TurnGetState() == TURN_APPROACH);
    assert(motorLeft == TURN_APPROACH_SPEED);
    assert(motorRight == TURN_APPROACH_SPEED);
}

static void CheckBusy(TurnState expected)
{
    unsigned before = motorWrites;
    unsigned beforeEncoders = encoderReads;
    assert(!TurnStart(TURN_LEFT));
    assert(!TurnStart(TURN_RIGHT));
    TurnReset(); /* Cannot discard a turn that is still in progress. */
    assert(TurnGetState() == expected);
    assert(motorWrites == before);
    assert(encoderReads == beforeEncoders);
}

static void CheckHeldResult(TurnState expected)
{
    unsigned before;
    unsigned savedReads[SENSOR_COUNT];
    unsigned beforeEncoders = encoderReads;
    assert(motorLeft == 0 && motorRight == 0);
    assert(!TurnStart(TURN_LEFT));
    assert(!TurnStart(TURN_RIGHT));
    memcpy(savedReads, reads, sizeof reads);
    before = motorWrites;
    ExpectTicks(4, expected);
    assert(motorWrites == before + 4);
    assert(motorLeft == 0 && motorRight == 0);
    assert(memcmp(savedReads, reads, sizeof reads) == 0);
    TurnReset();
    assert(TurnGetState() == TURN_IDLE);
    before = motorWrites;
    ExpectTicks(4, TURN_IDLE);
    TurnReset();
    assert(motorWrites == before); /* IDLE also leaves motors alone on Reset. */
    assert(memcmp(savedReads, reads, sizeof reads) == 0);
    assert(encoderReads == beforeEncoders);
}

static void TestDirection(TurnDirection direction)
{
    Begin(direction);
    CheckBusy(TURN_APPROACH);
    /* All six sensors can be black; only actual encoder travel ends approach. */
    sensors[SENSOR_FL] = sensors[SENSOR_FR] = 1;
    sensors[SENSOR_BM_L] = sensors[SENSOR_BM_R] = 1;
    ExpectTicks(TURN_CONFIRM_READINGS + 1, TURN_APPROACH);
    assert(motorLeft == TURN_APPROACH_SPEED);
    Advance(TURN_APPROACH_COUNTS - 1, TURN_APPROACH_COUNTS - 1);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_APPROACH);
    Advance(1, 1);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_APPROACH);
    Advance(-1, -1); /* Falling below target resets confirmation. */
    ExpectTicks(1, TURN_APPROACH);
    Advance(1, 1);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_APPROACH);
    assert(Tick() == TURN_LEAVE_LINE);
    assert(motorLeft == (direction == TURN_LEFT ?
                         -TURN_ROTATE_SPEED : TURN_ROTATE_SPEED));
    assert(motorRight == -motorLeft);
    CheckBusy(TURN_LEAVE_LINE);

    ExpectTicks(TURN_CONFIRM_READINGS + 2, TURN_LEAVE_LINE);
    Middle(0, 1);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_LEAVE_LINE);
    Middle(1, 0);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_LEAVE_LINE);
    Middle(0, 0);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_LEAVE_LINE);
    Middle(1, 1); /* Brief background glitch must not arm reacquisition. */
    ExpectTicks(TURN_CONFIRM_READINGS + 1, TURN_LEAVE_LINE);
    Middle(0, 0);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_LEAVE_LINE);
    assert(Tick() == TURN_FIND_LINE);
    CheckBusy(TURN_FIND_LINE);

    Middle(1, 0);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_FIND_LINE);
    Middle(0, 1);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_FIND_LINE);
    Middle(1, 1);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_FIND_LINE);
    Middle(0, 1); /* Brief black pair must not complete the turn. */
    ExpectTicks(1, TURN_FIND_LINE);
    Middle(1, 1);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_FIND_LINE);
    assert(Tick() == TURN_DONE);
    assert(reads[SENSOR_FL] == 0 && reads[SENSOR_FR] == 0);
    assert(reads[SENSOR_BM_L] == 0 && reads[SENSOR_BM_R] == 0);
    CheckHeldResult(TURN_DONE);
}

static void TestTimeout(TurnDirection direction, TurnState stuckState)
{
    Begin(direction);
    if (stuckState != TURN_APPROACH) {
        Advance(TURN_APPROACH_COUNTS, TURN_APPROACH_COUNTS);
        ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_APPROACH);
        assert(Tick() == TURN_LEAVE_LINE);
    }
    if (stuckState == TURN_FIND_LINE) {
        Middle(0, 0);
        ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_LEAVE_LINE);
        assert(Tick() == TURN_FIND_LINE);
    }
    /* In LEAVE_LINE this is black forever: original line cannot finish. */
    while (updates + 1 < TURN_TIMEOUT_MS / TURN_UPDATE_MS) {
        assert(Tick() == stuckState);
        assert(motorLeft != 0 && motorRight != 0);
    }
    assert(Tick() == TURN_FAULT); /* Exact total deadline, never reset by state. */
    assert(updates == TURN_TIMEOUT_MS / TURN_UPDATE_MS);
    CheckHeldResult(TURN_FAULT);
}

static void TestDeadlineWins(void)
{
    Begin(TURN_LEFT);
    Advance(TURN_APPROACH_COUNTS, TURN_APPROACH_COUNTS);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_APPROACH);
    assert(Tick() == TURN_LEAVE_LINE);
    Middle(0, 0);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_LEAVE_LINE);
    assert(Tick() == TURN_FIND_LINE);
    while (updates < TURN_TIMEOUT_MS / TURN_UPDATE_MS - TURN_CONFIRM_READINGS) {
        assert(Tick() == TURN_FIND_LINE);
    }
    Middle(1, 1);
    ExpectTicks(TURN_CONFIRM_READINGS - 1, TURN_FIND_LINE);
    assert(Tick() == TURN_FAULT);
    CheckHeldResult(TURN_FAULT);
}

static void TestReverseAndStalledWheel(void)
{
    Begin(TURN_RIGHT);
    Advance(-TURN_APPROACH_COUNTS, -TURN_APPROACH_COUNTS);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_APPROACH);
    /* Left is now a full target ahead; right is still a target behind. */
    Advance(2 * TURN_APPROACH_COUNTS, 0);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_APPROACH);
    Advance(0, TURN_APPROACH_COUNTS);
    ExpectTicks(TURN_CONFIRM_READINGS, TURN_APPROACH); /* Right only at baseline. */
    while (updates + 1 < TURN_TIMEOUT_MS / TURN_UPDATE_MS) {
        assert(Tick() == TURN_APPROACH);
    }
    assert(Tick() == TURN_FAULT);
    CheckHeldResult(TURN_FAULT);
}

int main(void)
{
    unsigned before;
    assert(TurnGetState() == TURN_IDLE);
    motorLeft = 37;
    motorRight = 42;
    before = motorWrites;
    ExpectTicks(5, TURN_IDLE);
    TurnReset();
    assert(!TurnStart((TurnDirection)99));
    assert(motorWrites == before);
    assert(motorLeft == 37 && motorRight == 42);
    assert(encoderReads == 0);
    assert(SENSOR_COUNT == 6);

    TestDirection(TURN_LEFT);
    TestDirection(TURN_RIGHT);
    TestTimeout(TURN_LEFT, TURN_APPROACH);
    TestTimeout(TURN_RIGHT, TURN_APPROACH);
    TestTimeout(TURN_LEFT, TURN_LEAVE_LINE);
    TestTimeout(TURN_RIGHT, TURN_LEAVE_LINE);
    TestTimeout(TURN_LEFT, TURN_FIND_LINE);
    TestTimeout(TURN_RIGHT, TURN_FIND_LINE);
    TestDeadlineWins();
    TestReverseAndStalledWheel();
    puts("PASS: six sensors, left/right, encoder approach/wrap/reverse/stall,");
    puts("      original-line guard, debounce, ownership, result locking, timeout.");
    puts("Host mocks only; no physical robot testing.");
    return 0;
}
