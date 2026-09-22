#include "turn.h"

/* Six-sensor layout, viewed from above:
 * Front:  FL   FM_L   FM_R   FR
 * Back:       BM_L   BM_R
 *
 * Main detects the intersection with FL/FR and calls TurnStart once.
 * Encoders position the robot for rotation; FM_L/FM_R detect line departure
 * and reacquisition. The rear middle sensors are not used by this module.
 *
 * Normal sequence: IDLE -> APPROACH -> LEAVE_LINE -> FIND_LINE -> DONE.
 * Any moving stage can time out to FAULT. Call TurnUpdate every 5 ms.
 */

/* File-scope static variables are private to this file and retain their values
 * between calls, allowing each update to continue where the last one stopped.
 */
static TurnState state = TURN_IDLE;              /* Current turning stage. */
static TurnDirection turnDirection = TURN_LEFT;  /* Direction saved at start. */
static uint16_t consecutive = 0;                 /* Successful checks in a row. */
static uint32_t elapsedTicks = 0;                /* 5 ms updates since start. */

/* Previous raw encoder readings and accumulated travel since TurnStart.
 * The travel totals use wider integers so hardware counter wraps do not limit
 * the software totals. Travel is measured in encoder counts, not millimetres.
 */
static int16_t previousLeftCount;
static int16_t previousRightCount;
static int64_t leftTravelCounts;
static int64_t rightTravelCounts;

/* Handle the existing 16-bit hardware counters wrapping in either direction.
 * Subtract after widening to avoid signed overflow or narrowing conversions.
 * Example: 32767 -> -32768 is +1 count, corrected by adding 65536.
 * Assumes fewer than 32768 counts of movement between consecutive readings.
 */
static int32_t EncoderDelta(int16_t current, int16_t previous)
{
    int32_t delta = (int32_t)current - (int32_t)previous;
    if (delta > 32767L) delta -= 65536L;
    if (delta < -32768L) delta += 65536L;
    return delta;
}

static bool ApproachDistanceReached(void)
{
    int16_t left;
    int16_t right;
    ReadWheelEncoderCounts(&left, &right);
    /* Add only movement since the previous update. Each sign setting converts
     * that wheel's raw encoder direction into positive forward travel.
     */
    leftTravelCounts += EncoderDelta(left, previousLeftCount) *
                        TURN_LEFT_ENCODER_FORWARD_SIGN;
    rightTravelCounts += EncoderDelta(right, previousRightCount) *
                         TURN_RIGHT_ENCODER_FORWARD_SIGN;
    /* Save this sample so the next update does not count the same travel again. */
    previousLeftCount = left;
    previousRightCount = right;

    /* Require BOTH wheels to travel forward far enough. One spinning wheel
     * must not substitute for a stalled wheel. Reverse travel subtracts.
     * Equal approach speeds assume matched motion; this is not steering PID.
     */
    return leftTravelCounts >= TURN_APPROACH_COUNTS &&
           rightTravelCounts >= TURN_APPROACH_COUNTS;
}

/* Debounce a transition: require TURN_CONFIRM_READINGS successes in a row.
 * With the default of 3, true/true/false resets the count; true/true/true passes.
 * A failed reading breaks the run; confirmations never cross state changes.
 * This also confirms that both encoder totals remain above the approach target.
 */
static bool Confirm(bool condition)
{
    if (!condition) {
        consecutive = 0;
        return false;
    }
    ++consecutive;
    if (consecutive >= TURN_CONFIRM_READINGS) {
        consecutive = 0;
        return true;
    }
    return false;
}

/* SetMotorSpeed(left, right) accepts signed percentages: +forward, -reverse.
 * The hardware adapter must use magnitude for PWM and sign for direction.
 * The normal controller must not write motors while this module is non-IDLE.
 */
static void ApplyMotorCommand(void)
{
    switch (state) {
    case TURN_IDLE:
        break; /* Normal controller owns the motors. */
    case TURN_APPROACH:
        /* Equal low duties drive forward toward the calibrated pivot position. */
        SetMotorSpeed(TURN_APPROACH_SPEED, TURN_APPROACH_SPEED);
        break;
    case TURN_LEAVE_LINE:
    case TURN_FIND_LINE:
        if (turnDirection == TURN_LEFT) {
            /* Left wheel backward, right wheel forward: rotate left. */
            SetMotorSpeed(-TURN_ROTATE_SPEED, TURN_ROTATE_SPEED);
        } else {
            /* Left wheel forward, right wheel backward: rotate right. */
            SetMotorSpeed(TURN_ROTATE_SPEED, -TURN_ROTATE_SPEED);
        }
        break;
    case TURN_DONE:
    case TURN_FAULT:
    default:
        SetMotorSpeed(0, 0);
        break;
    }
}

/* Called once per junction by main. True means this module now owns motors. */
bool TurnStart(TurnDirection direction)
{
    /* Reject a busy turn, an unhandled result, or an invalid direction. */
    if (state != TURN_IDLE ||
        (direction != TURN_LEFT && direction != TURN_RIGHT)) {
        return false;
    }
    /* Start a fresh software measurement for this turn. */
    turnDirection = direction;
    consecutive = 0;
    elapsedTicks = 0;
    leftTravelCounts = 0;
    rightTravelCounts = 0;
    /* Save baselines without resetting hardware counters used by other code. */
    ReadWheelEncoderCounts(&previousLeftCount, &previousRightCount);
    state = TURN_APPROACH;
    ApplyMotorCommand();
    return true;
}

/* Main schedules this every 5 ms. Each call checks the current stage and
 * returns immediately; there are no delays or loops waiting for the robot.
 */
TurnState TurnUpdate(void)
{
    uint8_t frontLeft;
    uint8_t frontRight;

    if (state == TURN_IDLE) {
        return state;
    }
    if (state == TURN_DONE || state == TURN_FAULT) {
        ApplyMotorCommand(); /* Latch result and keep commanding stop. */
        return state;
    }

    /* One timeout covers the ENTIRE approach + rotation. The deadline wins
     * over a sensor transition on the same tick. No delays or busy loops.
     * This measures scheduled calls; a stalled scheduler cannot be detected
     * with these hardware interfaces (use a system watchdog separately).
     */
    /* For the defaults: 4000 ms / 5 ms = 800 updates before timeout. */
    ++elapsedTicks;
    if (elapsedTicks >= TURN_TIMEOUT_MS / TURN_UPDATE_MS) {
        consecutive = 0;
        state = TURN_FAULT;
        ApplyMotorCommand();
        return state;
    }

    switch (state) {
    case TURN_APPROACH:
        /* The six-sensor board has no rear outer branch sensors. Travel is
         * measured from TurnStart, using the existing wheel encoders instead.
         * TURN_APPROACH_COUNTS must be calibrated for the front trigger's
         * distance from the desired wheel-axle pivot position.
         */
        if (Confirm(ApproachDistanceReached())) {
            state = TURN_LEAVE_LINE;
        }
        break;

    case TURN_LEAVE_LINE:
        /* These are the front middle pair (Q6 and Q5 in the supplied layout).
         * ReadSensor normalizes the hardware values: black=1, background=0.
         */
        frontLeft = ReadSensor(SENSOR_FM_L);
        frontRight = ReadSensor(SENSOR_FM_R);
        /* BOTH must be background together for several consecutive ticks.
         * Staying black on the original line must NEVER complete a turn.
         * A mixed pair (0,1) or (1,0) is not enough to advance.
         */
        if (Confirm(frontLeft == 0U && frontRight == 0U)) {
            state = TURN_FIND_LINE;
        }
        break;

    case TURN_FIND_LINE:
        frontLeft = ReadSensor(SENSOR_FM_L);
        frontRight = ReadSensor(SENSOR_FM_R);
        /* Assumes the first sustained black pair after the clear gap is the
         * requested branch. This detects a line, not an exact heading.
         * A mixed pair or brief black pulse resets confirmation. Both sensors
         * must fit on the branch long enough at the selected rotation speed.
         */
        if (Confirm(frontLeft == 1U && frontRight == 1U)) {
            state = TURN_DONE;
        }
        break;

    default:
        state = TURN_FAULT;
        break;
    }

    ApplyMotorCommand(); /* Rotation/stop takes effect on the transition tick. */
    return state;
}

/* Read-only status query: does not advance the turn or change motor commands. */
TurnState TurnGetState(void)
{
    return state;
}

/* Main handles success/failure first, then acknowledges it with TurnReset.
 * Keep the motors stopped while releasing ownership to the normal controller.
 * Calls during a moving stage are ignored; Reset is not an emergency-stop API.
 */
void TurnReset(void)
{
    if (state == TURN_DONE || state == TURN_FAULT) {
        SetMotorSpeed(0, 0);
        consecutive = 0;
        elapsedTicks = 0;
        state = TURN_IDLE;
    }
}
