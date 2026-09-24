#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include <project.h>
#include <stdint.h>

typedef enum {
    ROBOT_STATE_SENSOR_FAULT, /* Missing, unknown, or invalid sensor data. */
    ROBOT_STATE_LINE_LOST,    /* All six sensors see white. */
    ROBOT_STATE_FOLLOW_LINE,  /* Middle/rear sensor sees black, no outer black. */
    ROBOT_STATE_LEFT_BRANCH,  /* Front-left outer sensor sees black. */
    ROBOT_STATE_RIGHT_BRANCH, /* Front-right outer sensor sees black. */
    ROBOT_STATE_JUNCTION      /* Both front outer sensors see black. */
} RobotState;

/* Pass frame.state from the existing SensorFrame, in Q1..Q6 channel order.
 * Values use sensor.h's convention: BLACK=0, WHITE=1, UNKNOWN=2.
 * Requires six readable entries; NULL returns SENSOR_FAULT.
 * Call on a fresh frame, outside an active turn. This classifies one sample;
 * the caller owns confirmation, stale-data handling, route choice and motors.
 */
RobotState RobotDecideState(uint8_t sensorStates[6]);

#endif
