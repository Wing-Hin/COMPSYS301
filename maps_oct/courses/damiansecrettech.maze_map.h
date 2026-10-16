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
 * This placeholder is maps_oct/damiansecrettech.map, turned a quarter turn
 * into the robot's frame the same way as map_1: robot (r, c) = file row c,
 * file column 18 - r. Two separate tracks that never meet. Left (cols 1-3):
 * a spine with a one-cell stub every block, alternating sides. Right (cols
 * 5-14): a loop of the south border, a 17-block bare straight up column 7
 * and the north border, plus a 3-wide band of solid lattice in columns 10-12
 * -- crossroads at almost every cell -- and dead ends at (0,5), (18,5),
 * (0,14) and (18,14). The course runs on the right-hand track. */
static const signed char maze[][MAZE_COLS] = {
    {0,0,1,1,0,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0},
    {0,1,1,0,0,0,0,1,0,0,1,1,1,0,0},
    {0,0,1,1,0,1,1,1,1,1,1,1,1,1,1}
};

#define MAZE_ROWS ((int)(sizeof maze / sizeof maze[0]))

/* Start, then the four waypoints, then the target: {row, column}, row 0 at the
 * top, column 0 at the left. Six entries, visited in this order.
 *
 * If staff give fewer or more waypoints, change MAZE_NPOINTS and the list to
 * match -- the planner walks whatever is here, in order, and nothing else
 * assumes six. */
/* TEST COURSE on damiansecrettech, not a staff map, in the demo's shape:
 * start, four waypoints, target -- all on the right-hand track, since the
 * left one cannot be reached from it. Found by searching with the planner:
 * a dead-end 180, the dense crossroads band (a landmark every block, both
 * sides), 180s at a crossroads and at a T inside it, and a 180 five blocks
 * down the bare straight where there is nothing to confirm position. 83
 * blocks. Put the staff points back for the demo. */
#define MAZE_NPOINTS 6
static const signed char mazePoint[MAZE_NPOINTS][2] = {
    { 18,  5 },     /* start      -- dead end at the west end of the south */
                    /*               row, facing east (file row 5, col 0) */
    { 18, 14 },     /* waypoint 1 -- 8L  180 at the other end's dead end   */
    {  3, 10 },     /* waypoint 2 -- 9L  180 on the band's west edge       */
    { 13, 11 },     /* waypoint 3 -- 9L  180 at a crossroads in the band   */
    {  5,  7 },     /* waypoint 4 -- 8L  180 five blocks down the bare     */
                    /*               straight: stopped on distance alone  */
    { 12, 11 }      /* TARGET     -- in the band                           */
};

/* Which way the robot faces while it sits on the start.
 * One of HDG_NORTH, HDG_EAST, HDG_SOUTH, HDG_WEST.
 * North is toward row 0, east is toward the last column. */
#define MAZE_START_HEADING  HDG_EAST

#endif /* MAZE_MAP_H */
