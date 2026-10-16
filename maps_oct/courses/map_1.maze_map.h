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
 * This placeholder is maps_oct/map_1.map, turned a quarter turn into the
 * robot's frame: robot (r, c) = file row c, file column 18 - r. So the
 * file's top-left corner is the robot's start (18,0), the file's top row is
 * the robot's left border (column 0), and the robot faces along it, toward
 * the file's right-hand end. */
static const signed char maze[][MAZE_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,0,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,1,0,1,0,0,0,0,0,1},
    {1,1,1,0,1,0,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,1,0,1,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
    {1,1,1,0,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

#define MAZE_ROWS ((int)(sizeof maze / sizeof maze[0]))

/* Start, then the four waypoints, then the target: {row, column}, row 0 at the
 * top, column 0 at the left. Six entries, visited in this order.
 *
 * If staff give fewer or more waypoints, change MAZE_NPOINTS and the list to
 * match -- the planner walks whatever is here, in order, and nothing else
 * assumes six. */
/* TEST COURSE on map_1, not a staff map: seven waypoints chosen so this
 * map exercises every kind of decision it has. There is no crossroads on
 * map_1, so no 7; everything else is here. The comment on each says what the
 * robot does on arriving there. Seven turns on the way are plain bends, not
 * waypoints: (16,2) (14,2) (14,4) (8,2) (16,4) (16,6) (12,6).
 * Put the staff points back for the demo. */
#define MAZE_NPOINTS 9
static const signed char mazePoint[MAZE_NPOINTS][2] = {
    { 18,  0 },     /* start      -- file's top-left corner, facing along  */
                    /*               the top row (robot north)            */
    {  8,  4 },     /* waypoint 1 -- 8R  180 at the side of a T, swung     */
                    /*               through bare floor                   */
    {  9,  4 },     /* waypoint 2 -- 8L  180 in a corridor                 */
    { 10,  2 },     /* waypoint 3 -- 8L  180 at a dead end                 */
                    /*   (18,0)      1L  bottom-left corner on the way     */
    {  8,  8 },     /* waypoint 4 -- 9L  180 at a T's stem, both sides     */
                    /*               open: ignore the first line          */
    { 12,  8 },     /* waypoint 5 -- 5L  T met head-on                     */
    { 12, 10 },     /* waypoint 6 -- 2F  corridor: no side lines           */
    { 12, 12 },     /* waypoint 7 -- 8L  180 at a bend                     */
    { 12,  9 }      /* TARGET     -- back along the corridor               */
};

/* Which way the robot faces while it sits on the start.
 * One of HDG_NORTH, HDG_EAST, HDG_SOUTH, HDG_WEST.
 * North is toward row 0, east is toward the last column. */
#define MAZE_START_HEADING  HDG_NORTH

#endif /* MAZE_MAP_H */
