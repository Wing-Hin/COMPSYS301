#include "robot_state.h"
#include "sensor.h"

#include <stddef.h>

#if N_SENSORS != 6
#error "RobotDecideState expects the six-sensor board"
#endif

RobotState RobotDecideState(const uint8_t sensorStates[6])
{
    unsigned int i;
    unsigned int blackCount = 0;
    int leftBranch;
    int rightBranch;

    if (sensorStates == NULL) {
        return ROBOT_STATE_SENSOR_FAULT;
    }

    /* Check validity first: UNKNOWN must not be mistaken for white. */
    for (i = 0; i < N_SENSORS; ++i) {
        if (sensorStates[i] != SENSOR_BLACK &&
            sensorStates[i] != SENSOR_WHITE) {
            return ROBOT_STATE_SENSOR_FAULT;
        }
        if (sensorStates[i] == SENSOR_BLACK) {
            ++blackCount;
        }
    }

    /* Same physical mapping as the Turning hardware adapter:
     * front: Q3 (outer left), Q6, Q5, Q4 (outer right)
     * rear:                  Q1, Q2
     * Outer black readings indicate possible branches, not a chosen route.
     */
    leftBranch = sensorStates[Q3] == SENSOR_BLACK;
    rightBranch = sensorStates[Q4] == SENSOR_BLACK;

    /* Check both sides before either side individually. */
    if (leftBranch && rightBranch) {
        return ROBOT_STATE_JUNCTION;
    }
    if (leftBranch) {
        return ROBOT_STATE_LEFT_BRANCH;
    }
    if (rightBranch) {
        return ROBOT_STATE_RIGHT_BRANCH;
    }
    if (blackCount == 0) {
        return ROBOT_STATE_LINE_LOST;
    }

    return ROBOT_STATE_FOLLOW_LINE;
}
