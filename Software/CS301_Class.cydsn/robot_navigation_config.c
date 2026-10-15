#include "robot_navigation_config.h"

/* REPLACE with your map, row by row: 0 road, 1 wall. This blocked cell is
 * deliberately not a demo route. Update rows/cols below when you enter it. */
static const uint8_t routeMap[] = {1};

const RobotNavigationConfig robotNavigationConfig = {
    routeMap,
    1, 1,               /* rows, cols: must match routeMap */
    {0, 0},             /* start: row, column */
    {0, 0},             /* goal: row, column */
    NAV_NORTH,          /* actual starting heading; must face first path step */
    100,                /* 9 cm centre-to-centre grid step; verify on robot */
    0,                  /* PLACEHOLDER: landmark tolerance (< half a cell) */
    0,                  /* calibrate travel already covered when turn ends */
    15000UL,            /* timeout per straight segment, tune for longest one */
    -1, 1,              /* forward encoder signs: verify on your robot */
    25                  /* line-follow speed percentage */
};

/* Initial estimate: 0.09 / (pi * 0.065) * 228 = approximately 100 counts.
 * Uses the 9 cm centre-to-centre spacing, not the 10.5 cm outer-to-outer width.
 * Verify wheel diameter, encoder counts and actual travel on the robot.
 * The blocked placeholder map still makes Init stop with ROBOT_NAV_CONFIG.
 */
