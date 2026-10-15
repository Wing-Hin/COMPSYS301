#include "robot_navigation.h"
#include "turn.h"
#include <assert.h>
#include <stdio.h>

SensorFrame controlSensorFrame;
static int16_t leftCounter, rightCounter;
static unsigned int startCalls, followCalls, stopCalls, updateCalls;
static TurnState turnState;
static TurnDirection turnDirection;
int16_t QuadDec_M1_GetCounter(void) { return leftCounter; }
int16_t QuadDec_M2_GetCounter(void) { return rightCounter; }
void SetMotorSpeed(int16_t left, int16_t right)
{ assert(left == 0 && right == 0); ++stopCalls; }
void straightFromFrame(uint8 speed, const SensorFrame *frame)
{ assert(speed > 0 && frame->fresh && turnState == TURN_IDLE); ++followCalls; }
void TurnAbort(void) { turnState = TURN_FAULT; }
void TurnReset(void) { turnState = TURN_IDLE; }
bool TurnStart(TurnDirection direction)
{
    assert(turnState == TURN_IDLE);
    ++startCalls; turnDirection = direction; turnState = TURN_APPROACH;
    return true;
}
TurnState TurnUpdate(void) { ++updateCalls; return turnState; }

static SensorFrame frame(bool left, bool right)
{
    SensorFrame f = {{SENSOR_BLACK, SENSOR_BLACK, SENSOR_WHITE,
                      SENSOR_WHITE, SENSOR_BLACK, SENSOR_BLACK}, 1};
    f.state[Q3] = left ? SENSOR_BLACK : SENSOR_WHITE;
    f.state[Q4] = right ? SENSOR_BLACK : SENSOR_WHITE;
    return f;
}
static void sample(uint32_t now, bool left, bool right)
{ SensorFrame f = frame(left, right); RobotNavigationUpdate(&f, now); }
static RobotNavigationConfig config(const uint8_t *map, uint16_t rows,
                                    uint16_t cols, BfsCell start, BfsCell goal)
{
    RobotNavigationConfig c = {map, rows, cols, start, goal, NAV_EAST,
                               100, 20, 0, 1000, 1, 1, 25};
    return c;
}
static void clear_intersection(uint32_t now)
{ sample(now + 1, false, false); sample(now + 2, false, false); sample(now + 3, false, false); }

static void test_left_turn_and_finish(void)
{
    const uint8_t map[] = {1,1,0, 0,0,0};
    RobotNavigationConfig c = config(map, 2, 3, (BfsCell){1,0}, (BfsCell){0,2});
    unsigned int oldFollow, oldStarts;
    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0));
    clear_intersection(0);
    leftCounter = rightCounter = 200;
    oldStarts = startCalls;
    sample(4, true, false); sample(5, true, false);
    assert(startCalls == oldStarts);
    sample(6, true, false);
    assert(startCalls == oldStarts + 1 && turnDirection == TURN_LEFT);
    assert(RobotNavigationGet()->heading == NAV_EAST);
    oldFollow = followCalls;
    sample(11, true, false); sample(16, true, false);
    assert(followCalls == oldFollow && startCalls == oldStarts + 1);
    turnState = TURN_DONE;
    sample(21, false, false);
    assert(RobotNavigationGet()->heading == NAV_NORTH && followCalls == oldFollow);
    leftCounter = rightCounter = 300;
    sample(22, false, false);
    assert(RobotNavigationGet()->state == NAV_DONE);
    assert(RobotNavigationGetError() == ROBOT_NAV_OK);
    assert(stopCalls > 0);
}

static void test_straight_junction(void)
{
    const uint8_t map[] = {1,1,0,1,1, 0,0,0,0,0};
    RobotNavigationConfig c = config(map, 2, 5, (BfsCell){1,0}, (BfsCell){1,4});
    unsigned int oldStarts = startCalls;
    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0)); clear_intersection(0);
    leftCounter = rightCounter = 200;
    sample(4, true, false); sample(5, true, false); sample(6, true, false);
    assert(RobotNavigationGet()->index == 2 && startCalls == oldStarts);
    sample(7, true, false); sample(8, true, false); sample(9, true, false);
    assert(RobotNavigationGet()->index == 2); /* Same junction held high. */
    leftCounter = rightCounter = 400;
    sample(10, false, false);
    assert(RobotNavigationGet()->state == NAV_DONE);
}

static void test_faults_and_encoder_wrap(void)
{
    const uint8_t line[] = {0,0};
    RobotNavigationConfig c = config(line, 1, 2, (BfsCell){0,0}, (BfsCell){0,1});
    SensorFrame stale = frame(false, false);
    c.cell_counts = 0;
    assert(!RobotNavigationInit(&c, 0));
    assert(RobotNavigationGetError() == ROBOT_NAV_CONFIG);
    c.cell_counts = 100;
    c.heading = NAV_WEST;
    assert(!RobotNavigationInit(&c, 0));
    assert(RobotNavigationGetError() == ROBOT_NAV_INITIAL_HEADING);
    c.heading = NAV_EAST;
    leftCounter = rightCounter = 32760;
    assert(RobotNavigationInit(&c, 0));
    leftCounter = rightCounter = -32676; /* +100 across signed 16-bit wrap. */
    sample(1, false, false);
    assert(RobotNavigationGet()->state == NAV_DONE);
    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0));
    leftCounter = 100; rightCounter = 0;
    sample(1, false, false);
    assert(RobotNavigationGet()->state == NAV_FOLLOWING);
    stale.fresh = 0;
    RobotNavigationUpdate(&stale, 51);
    assert(RobotNavigationGetError() == ROBOT_NAV_SENSOR);
    sample(52, false, false);
    assert(RobotNavigationGet()->state == NAV_FAULT); /* Fault stays latched. */
    assert(RobotNavigationInit(&c, UINT32_MAX - 10));
    RobotNavigationUpdate(&stale, 40);
    assert(RobotNavigationGetError() == ROBOT_NAV_SENSOR);
    assert(RobotNavigationInit(&c, 0));
    sample(1000, false, false);
    assert(RobotNavigationGetError() == ROBOT_NAV_TIMEOUT);
}

static void test_missed_and_wrong_branch(void)
{
    const uint8_t map[] = {1,1,0, 0,0,0};
    RobotNavigationConfig c = config(map, 2, 3, (BfsCell){1,0}, (BfsCell){0,2});
    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0)); clear_intersection(0);
    leftCounter = rightCounter = 221;
    sample(4, false, false);
    assert(RobotNavigationGetError() == ROBOT_NAV_POSITION);
    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0)); clear_intersection(0);
    leftCounter = rightCounter = 200;
    sample(4, false, true); sample(5, false, true); sample(6, false, true);
    assert(RobotNavigationGetError() == ROBOT_NAV_BRANCH);
}

static void test_right_turn_faults(void)
{
    const uint8_t map[] = {0,0,0, 1,1,0};
    RobotNavigationConfig c = config(map, 2, 3, (BfsCell){0,0}, (BfsCell){1,2});
    unsigned int oldFollow;
    SensorFrame invalid = frame(false, false);
    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0)); clear_intersection(0);
    leftCounter = rightCounter = 200;
    sample(4, false, true); sample(5, false, true); sample(6, false, true);
    assert(turnDirection == TURN_RIGHT);
    oldFollow = followCalls;
    turnState = TURN_FAULT;
    sample(11, false, true);
    assert(RobotNavigationGetError() == ROBOT_NAV_TURN && followCalls == oldFollow);

    leftCounter = rightCounter = 0;
    assert(RobotNavigationInit(&c, 0)); clear_intersection(0);
    leftCounter = rightCounter = 200;
    sample(4, false, true); sample(5, false, true); sample(6, false, true);
    invalid.state[Q6] = SENSOR_UNKNOWN;
    RobotNavigationUpdate(&invalid, 7);
    assert(RobotNavigationGetError() == ROBOT_NAV_SENSOR);
    assert(turnState == TURN_FAULT); /* Abort an active turn immediately. */
}

int main(void)
{
    test_left_turn_and_finish(); test_straight_junction();
    test_faults_and_encoder_wrap(); test_missed_and_wrong_branch();
    test_right_turn_faults();
    puts("PASS: robot navigation handoff, debounce, destination, encoder wrap and stopping faults");
    return 0;
}
