#ifndef ROBOT_NAVIGATION_H
#define ROBOT_NAVIGATION_H

#include "sensor.h"
#include "../Lucas's algorithm code/navigation.h"

typedef struct {
    const uint8_t *map;
    uint16_t rows, cols;
    BfsCell start, goal;
    NavHeading heading;
    uint32_t cell_counts;       /* Forward encoder counts per grid step. */
    uint32_t arrival_window;    /* Allowed landmark position error in counts. */
    int32_t turn_exit_offset;   /* Progress into next segment after a turn. */
    uint32_t segment_timeout_ms;
    int8_t left_encoder_sign, right_encoder_sign;
    uint8_t speed;
} RobotNavigationConfig;

typedef enum {
    ROBOT_NAV_OK, ROBOT_NAV_CONFIG, ROBOT_NAV_NO_PATH,
    ROBOT_NAV_INITIAL_HEADING, ROBOT_NAV_SENSOR,
    ROBOT_NAV_POSITION, ROBOT_NAV_BRANCH, ROBOT_NAV_TIMEOUT,
    ROBOT_NAV_TURN, ROBOT_NAV_UTURN
} RobotNavigationError;

/* Single robot instance. Config and its map must remain alive and immutable.
 * Call Init only while stopped. Restart explicitly after fixing a fault.
 * Align the robot with its first path step before starting.
 */
bool RobotNavigationInit(const RobotNavigationConfig *config, uint32_t now);
void RobotNavigationUpdate(const SensorFrame *frame, uint32_t now);
const Navigation *RobotNavigationGet(void);
RobotNavigationError RobotNavigationGetError(void);

#endif
