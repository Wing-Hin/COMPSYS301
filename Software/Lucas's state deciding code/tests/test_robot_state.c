#include "../robot_state.h"
#include "sensor.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main(void)
{
    SensorFrame frame;
    unsigned int mask;
    unsigned int i;
    unsigned int counts[6] = {0};

    assert(RobotDecideState(NULL) == ROBOT_STATE_SENSOR_FAULT);

    /* Exhaust every valid pattern and check left/right mirror symmetry. */
    for (mask = 0; mask < 64; ++mask) {
        RobotState state;
        RobotState mirrored;
        uint8_t swap;

        for (i = 0; i < N_SENSORS; ++i) {
            frame.state[i] = (mask & (1U << i)) ? SENSOR_BLACK : SENSOR_WHITE;
        }
        state = RobotDecideState(frame.state);
        assert(state >= ROBOT_STATE_LINE_LOST && state <= ROBOT_STATE_JUNCTION);
        ++counts[state];

        swap = frame.state[Q3];
        frame.state[Q3] = frame.state[Q4];
        frame.state[Q4] = swap;
        swap = frame.state[Q6];
        frame.state[Q6] = frame.state[Q5];
        frame.state[Q5] = swap;
        swap = frame.state[Q1];
        frame.state[Q1] = frame.state[Q2];
        frame.state[Q2] = swap;
        mirrored = RobotDecideState(frame.state);
        if (state == ROBOT_STATE_LEFT_BRANCH) {
            assert(mirrored == ROBOT_STATE_RIGHT_BRANCH);
        } else if (state == ROBOT_STATE_RIGHT_BRANCH) {
            assert(mirrored == ROBOT_STATE_LEFT_BRANCH);
        } else {
            assert(mirrored == state);
        }
    }
    assert(counts[ROBOT_STATE_LINE_LOST] == 1);
    assert(counts[ROBOT_STATE_FOLLOW_LINE] == 15);
    assert(counts[ROBOT_STATE_LEFT_BRANCH] == 16);
    assert(counts[ROBOT_STATE_RIGHT_BRANCH] == 16);
    assert(counts[ROBOT_STATE_JUNCTION] == 16);

    for (i = 0; i < N_SENSORS; ++i) frame.state[i] = SENSOR_WHITE;
    assert(RobotDecideState(frame.state) == ROBOT_STATE_LINE_LOST);
    frame.state[Q6] = SENSOR_BLACK;
    frame.state[Q5] = SENSOR_BLACK;
    assert(RobotDecideState(frame.state) == ROBOT_STATE_FOLLOW_LINE);
    frame.state[Q3] = SENSOR_BLACK;
    assert(RobotDecideState(frame.state) == ROBOT_STATE_LEFT_BRANCH);
    frame.state[Q3] = SENSOR_WHITE;
    frame.state[Q4] = SENSOR_BLACK;
    assert(RobotDecideState(frame.state) == ROBOT_STATE_RIGHT_BRANCH);
    frame.state[Q3] = SENSOR_BLACK;
    assert(RobotDecideState(frame.state) == ROBOT_STATE_JUNCTION);

    /* Bad readings override even an otherwise obvious junction. */
    for (i = 0; i < N_SENSORS; ++i) {
        uint8_t original = frame.state[i];
        frame.state[i] = SENSOR_UNKNOWN;
        assert(RobotDecideState(frame.state) == ROBOT_STATE_SENSOR_FAULT);
        frame.state[i] = 255;
        assert(RobotDecideState(frame.state) == ROBOT_STATE_SENSOR_FAULT);
        frame.state[i] = original;
    }

    puts("robot_state: all tests passed");
    return 0;
}
