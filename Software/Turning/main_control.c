#include "main_control.h"

#if FOLLOW_SPEED < 1 || FOLLOW_SPEED > 100 || \
    FOLLOW_INNER_SPEED < 0 || FOLLOW_INNER_SPEED >= FOLLOW_SPEED
#error "Following duties must satisfy 0 <= inner < speed <= 100"
#endif
#if CORNER_CONFIRM_READINGS < 2 || CORNER_CONFIRM_READINGS > 65535U || \
    LINE_LOST_CONFIRM_READINGS < 2 || LINE_LOST_CONFIRM_READINGS > 65535U
#error "Confirmation counts must be 2..65535"
#endif

static MainControlState controllerState = MAIN_STOPPED;
static MainControlFault fault = MAIN_FAULT_NONE;
static uint16_t completedTurns;
static bool cornerArmed;
static int candidateDirection = -1;
static uint16_t candidateCount;
static uint16_t clearCount;
static uint16_t ambiguousCount;
static uint16_t lostCount;

void MainControlHalt(MainControlFault reason)
{
    fault = reason;
    controllerState = MAIN_FAULT;
    SetMotorSpeed(0, 0);
}

bool MainControlStart(void)
{
    TurnState turnState = TurnGetState();
    if (controllerState == MAIN_FOLLOWING || controllerState == MAIN_TURNING ||
        turnState == TURN_APPROACH || turnState == TURN_LEAVE_LINE ||
        turnState == TURN_FIND_LINE) {
        return false;
    }
    TurnReset();
    completedTurns = 0;
    candidateDirection = -1;
    candidateCount = clearCount = ambiguousCount = lostCount = 0;
    cornerArmed = true;
    fault = MAIN_FAULT_NONE;
    controllerState = MAIN_FOLLOWING;
    SetMotorSpeed(0, 0); /* First scheduled update chooses the driving command. */
    return true;
}

/* Basic non-blocking steering based on the unfinished line_follow.c draft's
 * idea of slowing one wheel. This uses the actual mapped middle sensors and
 * signed-duty adapter, not the draft's undefined names or RPM regulator.
 * A black left front sensor alone means line to the left: slow the left wheel.
 */
static void FollowLine(const uint8_t *s)
{
    if (s[SENSOR_FM_L] && s[SENSOR_FM_R]) {
        SetMotorSpeed(FOLLOW_SPEED, FOLLOW_SPEED);
    } else if (s[SENSOR_FM_L]) {
        SetMotorSpeed(FOLLOW_INNER_SPEED, FOLLOW_SPEED);
    } else if (s[SENSOR_FM_R]) {
        SetMotorSpeed(FOLLOW_SPEED, FOLLOW_INNER_SPEED);
    } else if (s[SENSOR_BM_L] && s[SENSOR_BM_R]) {
        /* Front row has passed the corner; rear pair still tracks the approach.
         * Creep forward while corner detection is being confirmed.
         */
        SetMotorSpeed(TURN_APPROACH_SPEED, TURN_APPROACH_SPEED);
    } else {
        /* No reliable front guidance: stop rather than guess a turn from one
         * rear sensor. Corner/line-loss checks continue on following ticks.
         */
        SetMotorSpeed(0, 0);
    }
}

MainControlState MainControlUpdate5ms(void)
{
    uint8_t s[SENSOR_COUNT];
    unsigned i;
    int direction = -1;
    TurnState turnState;

    if (controllerState == MAIN_STOPPED || controllerState == MAIN_FAULT) {
        SetMotorSpeed(0, 0);
        return controllerState;
    }

    /* Turning has exclusive motor ownership; never run following afterwards. */
    turnState = TurnUpdate();
    if (turnState == TURN_FAULT) {
        MainControlHalt(MAIN_FAULT_TURN);
        return controllerState;
    }
    if (turnState == TURN_DONE) {
        if (completedTurns < UINT16_MAX) ++completedTurns;
        TurnReset();
        controllerState = MAIN_FOLLOWING;
        /* Rearm only after a clear, centred section during normal following. */
        cornerArmed = false;
        clearCount = candidateCount = ambiguousCount = lostCount = 0;
        candidateDirection = -1;
        return controllerState; /* Remain stopped until next scheduled update. */
    }
    if (turnState != TURN_IDLE) {
        controllerState = MAIN_TURNING;
        return controllerState;
    }

    for (i = 0; i < SENSOR_COUNT; ++i) {
        s[i] = ReadSensor((SensorId)i);
    }

    if (cornerArmed) {
        /* Both outers black can indicate a junction or broad black patch.
         * The benchmark has no intersections, so never guess a direction.
         */
        if (s[SENSOR_FL] && s[SENSOR_FR]) {
            lostCount = 0;
            candidateCount = 0;
            candidateDirection = -1;
            if (++ambiguousCount >= CORNER_CONFIRM_READINGS) {
                MainControlHalt(MAIN_FAULT_AMBIGUOUS_CORNER);
            } else {
                SetMotorSpeed(0, 0);
            }
            return controllerState;
        }
        ambiguousCount = 0;

        /* Rear pair must still be centred on the incoming line. This reduces
         * mistaking a lateral tracking error for a right-angle corner. It is
         * a track-dependent pattern, not geometric proof of a 90-degree bend.
         */
        if (s[SENSOR_BM_L] && s[SENSOR_BM_R]) {
            if (s[SENSOR_FL] && !s[SENSOR_FR]) direction = TURN_LEFT;
            if (s[SENSOR_FR] && !s[SENSOR_FL]) direction = TURN_RIGHT;
        }
        if (direction < 0) {
            candidateDirection = -1;
            candidateCount = 0;
        } else {
            lostCount = 0; /* Centred rear pair breaks any line-loss sequence. */
            if (candidateDirection != direction) {
                candidateDirection = direction;
                candidateCount = 0;
            }
            if (++candidateCount >= CORNER_CONFIRM_READINGS) {
                cornerArmed = false;
                candidateCount = 0;
                if (TurnStart((TurnDirection)direction)) {
                    controllerState = MAIN_TURNING;
                } else {
                    MainControlHalt(MAIN_FAULT_REQUEST);
                }
                return controllerState; /* Never overwrite TurnStart's command. */
            }
            SetMotorSpeed(TURN_APPROACH_SPEED, TURN_APPROACH_SPEED);
            return controllerState;
        }
    } else {
        /* Ignore lingering corner detections until both outers are clear and
         * all four middle sensors are back on a centred straight segment.
         * Do not count clear samples observed during rotation.
         */
        if (!s[SENSOR_FL] && !s[SENSOR_FR] &&
            s[SENSOR_FM_L] && s[SENSOR_FM_R] &&
            s[SENSOR_BM_L] && s[SENSOR_BM_R]) {
            if (++clearCount >= CORNER_CONFIRM_READINGS) {
                cornerArmed = true;
                clearCount = 0;
            }
        } else {
            clearCount = 0;
        }
    }

    /* No front guidance plus no centred rear pair is an untrackable pattern.
     * Stop immediately; latch a fault only after consecutive bad observations.
     */
    if (!s[SENSOR_FM_L] && !s[SENSOR_FM_R] &&
        !(s[SENSOR_BM_L] && s[SENSOR_BM_R])) {
        if (++lostCount >= LINE_LOST_CONFIRM_READINGS) {
            MainControlHalt(MAIN_FAULT_LINE_LOST);
            return controllerState;
        }
    } else {
        lostCount = 0;
    }
    FollowLine(s);
    return controllerState;
}

MainControlState MainControlGetState(void) { return controllerState; }
MainControlFault MainControlGetFault(void) { return fault; }
uint16_t MainControlGetCompletedTurns(void) { return completedTurns; }
