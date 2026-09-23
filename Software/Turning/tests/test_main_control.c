#include "main_control.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t sensorValues[SENSOR_COUNT];
static int16_t encoderLeft, encoderRight, motorLeft, motorRight;
static unsigned motorWrites;

uint8_t ReadSensor(SensorId sensor) { return sensorValues[sensor]; }
void ReadWheelEncoderCounts(int16_t *left, int16_t *right)
{
    *left = encoderLeft;
    *right = encoderRight;
}
void SetMotorSpeed(int16_t left, int16_t right)
{
    assert(left >= -100 && left <= 100 && right >= -100 && right <= 100);
    motorLeft = left;
    motorRight = right;
    ++motorWrites;
}

static void Centred(void)
{
    memset(sensorValues, 0, sizeof sensorValues);
    sensorValues[SENSOR_FM_L] = sensorValues[SENSOR_FM_R] = 1;
    sensorValues[SENSOR_BM_L] = sensorValues[SENSOR_BM_R] = 1;
}

static void Ticks(unsigned count, MainControlState state)
{
    unsigned i;
    for (i = 0; i < count; ++i) {
        assert(MainControlUpdate5ms() == state);
        assert(MainControlGetState() == state);
    }
}

static void Trigger(TurnDirection direction)
{
    Centred();
    sensorValues[direction == TURN_LEFT ? SENSOR_FL : SENSOR_FR] = 1;
    Ticks(CORNER_CONFIRM_READINGS - 1, MAIN_FOLLOWING);
    assert(TurnGetState() == TURN_IDLE);
    assert(MainControlUpdate5ms() == MAIN_TURNING);
    assert(TurnGetState() == TURN_APPROACH);
    assert(motorLeft == TURN_APPROACH_SPEED && motorRight == TURN_APPROACH_SPEED);
}

static void Complete(TurnDirection direction, unsigned expectedTurns)
{
    unsigned before;
    /* Test counts stay well within int16_t over this eleven-turn course. */
    encoderLeft = (int16_t)(encoderLeft + TURN_APPROACH_COUNTS * TURN_LEFT_ENCODER_FORWARD_SIGN);
    encoderRight = (int16_t)(encoderRight + TURN_APPROACH_COUNTS * TURN_RIGHT_ENCODER_FORWARD_SIGN);
    Ticks(TURN_CONFIRM_READINGS, MAIN_TURNING);
    assert(TurnGetState() == TURN_LEAVE_LINE);
    before = motorWrites;
    Ticks(1, MAIN_TURNING);
    assert(motorWrites == before + 1); /* No following write during rotation. */
    assert(motorLeft == (direction == TURN_LEFT ? -TURN_ROTATE_SPEED : TURN_ROTATE_SPEED));
    assert(motorRight == -motorLeft);
    sensorValues[SENSOR_FM_L] = sensorValues[SENSOR_FM_R] = 0;
    Ticks(TURN_CONFIRM_READINGS, MAIN_TURNING);
    assert(TurnGetState() == TURN_FIND_LINE);
    sensorValues[SENSOR_FM_L] = sensorValues[SENSOR_FM_R] = 1;
    Ticks(TURN_CONFIRM_READINGS - 1, MAIN_TURNING);
    assert(MainControlUpdate5ms() == MAIN_FOLLOWING);
    assert(motorLeft == 0 && motorRight == 0);
    assert(MainControlGetCompletedTurns() == expectedTurns);
    assert(TurnGetState() == TURN_IDLE);

    /* Same outer still black: no second turn. */
    Ticks(CORNER_CONFIRM_READINGS + 2, MAIN_FOLLOWING);
    assert(TurnGetState() == TURN_IDLE);
    /* Clear readings from the rotation did not rearm detection. */
    sensorValues[SENSOR_FL] = sensorValues[SENSOR_FR] = 0;
    Ticks(CORNER_CONFIRM_READINGS - 1, MAIN_FOLLOWING);
    sensorValues[SENSOR_FL] = 1;
    Ticks(CORNER_CONFIRM_READINGS, MAIN_FOLLOWING);
    assert(TurnGetState() == TURN_IDLE);
    Centred();
    Ticks(CORNER_CONFIRM_READINGS, MAIN_FOLLOWING);
}

static void AssertFault(MainControlFault reason)
{
    assert(MainControlGetState() == MAIN_FAULT);
    assert(MainControlGetFault() == reason);
    assert(motorLeft == 0 && motorRight == 0);
    Centred();
    Ticks(5, MAIN_FAULT);
    assert(motorLeft == 0 && motorRight == 0);
}

int main(void)
{
    unsigned i;
    assert(MainControlGetState() == MAIN_STOPPED);
    Ticks(1, MAIN_STOPPED);
    Centred();
    assert(MainControlStart());
    assert(!MainControlStart());
    Ticks(1, MAIN_FOLLOWING);
    assert(motorLeft == FOLLOW_SPEED && motorRight == FOLLOW_SPEED);
    sensorValues[SENSOR_FM_R] = 0;
    Ticks(1, MAIN_FOLLOWING);
    assert(motorLeft == FOLLOW_INNER_SPEED && motorRight == FOLLOW_SPEED);
    sensorValues[SENSOR_FM_L] = 0;
    sensorValues[SENSOR_FM_R] = 1;
    Ticks(1, MAIN_FOLLOWING);
    assert(motorLeft == FOLLOW_SPEED && motorRight == FOLLOW_INNER_SPEED);

    /* Brief and alternating outer detections must not accumulate. */
    Centred();
    sensorValues[SENSOR_FL] = 1;
    Ticks(CORNER_CONFIRM_READINGS - 1, MAIN_FOLLOWING);
    Centred();
    Ticks(1, MAIN_FOLLOWING);
    for (i = 0; i < 10; ++i) {
        sensorValues[SENSOR_FL] = (uint8_t)(i % 2);
        sensorValues[SENSOR_FR] = (uint8_t)(1 - i % 2);
        Ticks(1, MAIN_FOLLOWING);
    }
    Centred();
    sensorValues[SENSOR_FL] = 1;
    sensorValues[SENSOR_BM_R] = 0; /* Not centred: do not infer a corner. */
    Ticks(CORNER_CONFIRM_READINGS + 1, MAIN_FOLLOWING);
    assert(TurnGetState() == TURN_IDLE);

    for (i = 0; i < 11; ++i) {
        TurnDirection direction = i % 2 ? TURN_RIGHT : TURN_LEFT;
        Trigger(direction);
        Complete(direction, i + 1);
    }

    /* Both outer sensors black is ambiguous, not automatically left/right. */
    Centred();
    sensorValues[SENSOR_FL] = sensorValues[SENSOR_FR] = 1;
    Ticks(CORNER_CONFIRM_READINGS - 1, MAIN_FOLLOWING);
    assert(MainControlUpdate5ms() == MAIN_FAULT);
    AssertFault(MAIN_FAULT_AMBIGUOUS_CORNER);
    assert(MainControlStart());
    assert(MainControlGetCompletedTurns() == 0);

    memset(sensorValues, 0, sizeof sensorValues);
    Ticks(LINE_LOST_CONFIRM_READINGS - 1, MAIN_FOLLOWING);
    assert(motorLeft == 0 && motorRight == 0);
    Centred(); /* Recovery breaks the consecutive line-loss count. */
    Ticks(1, MAIN_FOLLOWING);
    memset(sensorValues, 0, sizeof sensorValues);
    Ticks(LINE_LOST_CONFIRM_READINGS - 1, MAIN_FOLLOWING);
    assert(MainControlUpdate5ms() == MAIN_FAULT);
    AssertFault(MAIN_FAULT_LINE_LOST);
    assert(MainControlStart());

    Trigger(TURN_RIGHT);
    Ticks(TURN_TIMEOUT_MS / TURN_UPDATE_MS - 1, MAIN_TURNING);
    assert(MainControlUpdate5ms() == MAIN_FAULT);
    AssertFault(MAIN_FAULT_TURN);
    assert(MainControlStart());

    Trigger(TURN_LEFT);
    MainControlHalt(MAIN_FAULT_SENSOR);
    AssertFault(MAIN_FAULT_SENSOR);
    assert(!MainControlStart()); /* Active turn was halted: board reset required. */
    puts("PASS: automatic corners, steering, glitches, eleven alternating turns,");
    puts("      rearming, motor ownership, ambiguous/lost-line/turn/hardware faults.");
    puts("Simulated controller verification only; no physical track testing.");
    return 0;
}
