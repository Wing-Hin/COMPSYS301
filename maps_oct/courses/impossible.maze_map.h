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
 * This placeholder is maps_oct/impossible.map, turned a quarter turn into
 * the robot's frame the same way as map_1: robot (r, c) = file row c, file
 * column 18 - r. One 159-cell line from a dead end at (16,0) to one at
 * (0,14), laid out as diagonal staircases: 135 of its cells are bends, no
 * straight is longer than 2, and there is not a single junction. Two small
 * separate fragments (top-left and bottom-right) cannot be reached. */
static const signed char maze[][MAZE_COLS] = {
    {1,0,1,1,1,0,0,0,1,1,1,1,0,0,1},
    {1,0,1,0,1,1,0,0,1,0,0,1,1,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,0,1,1,1},
    {0,0,0,1,1,0,1,1,0,1,1,0,0,0,0},
    {1,1,1,0,1,1,0,1,1,0,1,1,0,0,0},
    {1,0,1,1,0,1,1,0,1,1,0,1,1,0,0},
    {1,1,0,1,1,0,1,1,0,1,1,0,1,1,0},
    {0,1,1,0,1,1,0,1,1,0,1,1,0,1,1},
    {0,0,1,1,0,1,1,0,1,1,0,1,1,0,1},
    {0,0,0,1,1,0,1,1,0,1,1,0,1,1,1},
    {0,0,0,0,1,1,0,1,1,0,1,1,0,0,0},
    {1,1,1,1,0,1,1,0,1,1,0,1,1,0,0},
    {1,0,0,1,1,0,1,1,0,1,1,0,1,1,0},
    {1,1,0,0,1,1,0,1,1,0,1,1,0,1,1},
    {0,1,1,0,0,1,1,0,1,1,0,1,1,0,1},
    {0,0,1,1,0,0,1,1,0,1,1,0,1,1,1},
    {1,1,0,1,1,0,0,1,1,0,1,1,0,0,0},
    {0,1,0,0,1,0,0,0,1,1,0,1,0,1,1},
    {0,1,1,1,1,0,0,0,0,1,1,1,0,1,1}
};

#define MAZE_ROWS ((int)(sizeof maze / sizeof maze[0]))

/* Start, then the four waypoints, then the target: {row, column}, row 0 at the
 * top, column 0 at the left. Six entries, visited in this order.
 *
 * If staff give fewer or more waypoints, change MAZE_NPOINTS and the list to
 * match -- the planner walks whatever is here, in order, and nothing else
 * assumes six. */
/* TEST COURSE on impossible, not a staff map, in the demo's shape: start,
 * four waypoints, target, end to end along the staircase. A turn at almost
 * every grid point, nothing but bends to navigate by, and every waypoint is
 * either a lone straight cell with no side line (stopped on distance) or a
 * 180 at a bend. 170 blocks, 147 turns. Put the staff points back for the
 * demo. */
#define MAZE_NPOINTS 6
static const signed char mazePoint[MAZE_NPOINTS][2] = {
    { 16,  0 },     /* start      -- dead end, facing east (file row 0,    */
                    /*               col 2, facing down the file)         */
    { 18, 10 },     /* waypoint 1 -- 2F  a lone straight on the south edge */
    {  9,  7 },     /* waypoint 2 -- 8L  180 at a bend mid-staircase       */
    {  6,  4 },     /* waypoint 3 -- 8R  180 at a bend, 6 blocks back      */
    {  0,  3 },     /* waypoint 4 -- 2F  a lone straight on the north edge */
    {  0, 14 }      /* TARGET     -- the far dead end                      */
};

/* Which way the robot faces while it sits on the start.
 * One of HDG_NORTH, HDG_EAST, HDG_SOUTH, HDG_WEST.
 * North is toward row 0, east is toward the last column. */
#define MAZE_START_HEADING  HDG_EAST

#endif /* MAZE_MAP_H */
