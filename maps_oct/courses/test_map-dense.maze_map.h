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
 * This placeholder is maps_oct/test_map.map -- the map said to be like the
 * demo's -- turned a quarter turn into the robot's frame the same way as
 * map_1: robot (r, c) = file row c, file column 18 - r, so the file's
 * top-left corner is the robot's start (18,0). A real maze in one piece:
 * 166 cells, 15 dead ends, 45 T-junctions, 2 crossroads. */
static const signed char maze[][MAZE_COLS] = {
    {1,1,1,1,0,1,0,1,0,1,0,1,1,1,0},
    {1,0,1,0,0,1,0,1,1,1,1,1,0,1,1},
    {1,0,1,0,0,1,0,1,0,0,1,0,0,1,0},
    {1,1,1,0,0,1,0,1,0,0,1,0,0,1,1},
    {1,0,1,1,1,1,0,1,0,0,1,0,0,1,0},
    {1,0,0,1,0,1,1,1,0,0,1,1,0,1,0},
    {1,0,1,1,1,1,0,1,0,0,1,0,0,1,0},
    {1,1,1,1,0,1,0,1,0,0,1,1,0,1,1},
    {1,0,0,0,0,1,0,1,1,0,0,1,1,1,0},
    {1,0,0,1,1,1,0,1,0,1,0,0,0,1,0},
    {1,1,1,1,0,1,1,1,0,1,1,1,1,1,1},
    {0,0,0,1,0,1,0,1,0,1,0,0,1,0,1},
    {0,0,1,1,0,1,0,1,1,1,1,1,1,1,0},
    {0,0,1,0,0,0,0,1,0,0,0,1,0,1,1},
    {1,1,1,1,1,1,1,0,0,1,0,1,1,0,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,1,1,1},
    {1,0,1,1,1,0,1,0,0,0,1,0,0,1,0},
    {1,0,1,0,1,1,1,1,1,0,1,1,1,1,0},
    {1,1,1,1,1,0,0,0,1,1,1,0,0,1,0}
};

#define MAZE_ROWS ((int)(sizeof maze / sizeof maze[0]))

/* Start, then the four waypoints, then the target: {row, column}, row 0 at the
 * top, column 0 at the left. Six entries, visited in this order.
 *
 * If staff give fewer or more waypoints, change MAZE_NPOINTS and the list to
 * match -- the planner walks whatever is here, in order, and nothing else
 * assumes six. */
/* TEST COURSE on test_map -- turns packed close, then the bottom-right
 * loop. Same start; waypoints 1-4 from a search for turn DENSITY (the most
 * turns, most of them one grid point after the last), shuttling through the
 * winding middle of the map. Then waypoint 5 on the row-12 straight beside
 * the loop next to the start: the robot runs along that straight to it, 180s,
 * comes back along it, turns DOWN column 14 and runs the bottom row to stop
 * one cell short of the start. 107 blocks, 62 turns, five 180s.
 * Coordinates in the comments are as the mat is seen from the start: start
 * bottom-right, row 0 the far edge.
 * Put the staff points back for the demo. */
#define MAZE_NPOINTS 7
static const signed char mazePoint[MAZE_NPOINTS][2] = {
    { 18,  0 },     /* start      -- bottom-right (14,18), facing UP the   */
                    /*               right-hand column: the first leg     */
    {  5, 11 },     /* waypoint 1 -- (3,5)   */
    { 16, 13 },     /* waypoint 2 -- (1,16)  */
    {  6, 10 },     /* waypoint 3 -- (4,6)   */
    { 17, 13 },     /* waypoint 4 -- (1,17)  */
    { 12,  2 },     /* waypoint 5 -- (12,12) on the row-12 straight; 180   */
    { 17,  0 }      /* TARGET     -- (14,17), via down column 14           */
};

/* Which way the robot faces while it sits on the start.
 * One of HDG_NORTH, HDG_EAST, HDG_SOUTH, HDG_WEST.
 * North is toward row 0, east is toward the last column. */
#define MAZE_START_HEADING  HDG_EAST

#endif /* MAZE_MAP_H */
