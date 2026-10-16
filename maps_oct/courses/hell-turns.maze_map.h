/* ============================================================================
 * maze_map.h -- THE ONLY FILE YOU EDIT WHEN STAFF HAND YOU A MAP
 * ============================================================================
 * Three things live here and nothing else:
 *   1. maze[][]            the grid, exactly as pasted from Canvas
 *   2. mazePoint[][2]      start, then the four waypoints, then the target
 *   3. MAZE_START_HEADING  which way the robot faces while it sits on the start
 *
 * The row count is counted from the grid itself, so there is no second number
 * to keep in sync. Only MAZE_COLS has to match what you paste.
 *
 * 1 IS A LINE, 0 IS BARE FLOOR. This is the opposite of the convention the
 * older maps in this folder used, where 1 meant wall. The maps now come from
 * Canvas as the lines themselves: a grid of all 1s is the full lattice, every
 * square drawn, which is what the robot sees when nothing is cut away.
 *
 * Adjacent entries are MAZE_STEP_MM apart centre to centre, so one "block" in
 * a route is one step of the grid.
 *
 * MAZE_START_HEADING is the one thing the robot cannot work out for itself,
 * because you place it by hand. Get it wrong and the first instruction is
 * wrong, so the route is printed in plain English before anything moves --
 * check it against the robot in front of you before you let go.
 * ==========================================================================*/

#ifndef MAZE_MAP_H
#define MAZE_MAP_H

#define MAZE_COLS 15            /* entries per row in the grid below */

/* Centre-to-centre spacing of the lattice, in millimetres. */
#define MAZE_STEP_MM 95

/* The grid. 1 = line, 0 = no line. Paste straight from Canvas.
 * This placeholder is maps_oct/hell.map: the full lattice, every line
 * present. All 1s, so it reads the same whichever way the projector turns
 * it. */
static const signed char maze[][MAZE_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

#define MAZE_ROWS ((int)(sizeof maze / sizeof maze[0]))

/* Start, then the four waypoints, then the target: {row, column}, row 0 at the
 * top, column 0 at the left. Six entries, visited in this order.
 *
 * If staff give fewer or more waypoints, change MAZE_NPOINTS and the list to
 * match -- the planner walks whatever is here, in order, and nothing else
 * assumes six. */
/* TEST COURSE on hell.map, not a staff map: turns packed one grid point
 * apart, between long straights. Every corner is a waypoint, so every leg is
 * a straight line and A* has exactly one way to drive it -- the course is
 * what is written here, not whatever its tie-break prefers. 95 blocks, 40
 * turns (25 left, 15 right, two of them 180s). With P 1 the robot stops at
 * every one of these; leave P 0 for a run.
 * Put the staff points back for the demo. */
#define MAZE_NPOINTS 45
static const signed char mazePoint[MAZE_NPOINTS][2] = {
    {18, 0},                            /* start: bottom-left, facing east   */
    {18,12},                            /* long straight, 12 along the south */
    {17,12},{17,13},{16,13},            /* staircase: L R L R, a turn a block */
    {16,14},                            /* L at the east border (5L)         */
    { 4,14},                            /* long straight, 12 up the east     */
    { 4,13},{ 5,13},{ 5,12},{ 4,12},    /* square wave west along rows 4-5:  */
    { 4,11},{ 5,11},{ 5,10},{ 4,10},    /*   L R R L L R R L ... a turn      */
    { 4, 9},{ 5, 9},{ 5, 8},{ 4, 8},    /*   every block                     */
    { 4, 7},{ 5, 7},{ 5, 6},{ 4, 6},
    { 4, 5},
    { 0, 5},                            /* R, up to the north border (5L)    */
    { 0, 0},                            /* west along it; 1L at the corner   */
    {10, 0},                            /* long straight, 10 down the west   */
    {10, 1},{11, 1},{11, 2},{12, 2},    /* staircase down-right: L R L R ... */
    {12, 3},{13, 3},{13, 4},
    {13, 6},{12, 6},{12, 5},{13, 5},    /* a 1x1 loop: four lefts in a row   */
    {13, 6},
    {13,10},                            /* 180 mid-map (9L) ...              */
    {13, 8},                            /* ... back 2, L south               */
    {16, 8},{18, 8},                    /* 180 at the south border (9L)      */
    {15, 8},                            /* back up 3, R east                 */
    {15,12}                             /* TARGET                            */
};

/* Which way the robot faces while it sits on the start.
 * One of HDG_NORTH, HDG_EAST, HDG_SOUTH, HDG_WEST.
 * North is toward row 0, east is toward the last column. */
#define MAZE_START_HEADING  HDG_EAST

#endif /* MAZE_MAP_H */
