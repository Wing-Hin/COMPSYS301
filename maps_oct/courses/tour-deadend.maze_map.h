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
 * This placeholder is the TEST TOUR map: the full lattice with one dead-end
 * spur cut out at (16,7)-(16,8), and the same spur turned 180 degrees about
 * the centre at (2,6)-(2,7). The copy is there so the grid is the same
 * either way up: the projector's file (maps_oct/tour-deadend.map) is 15 x 19,
 * a quarter turn from this, and whichever way it is turned, the robot sees
 * this grid.
 *
 * GET THE ORIENTATION RIGHT. The grid must have the same shape as the mat AS
 * THE ROBOT SEES IT: row 0 is the far edge from the start, column 0 the left
 * edge. The mat is 15 lines across and 19 deep, so this is 19 rows of 15. An
 * earlier version had 15 rows of 19 -- the mat turned on its side -- and the
 * robot drove straight off the edge at 15 lines across, because the map said
 * there were 19. If Canvas gives the grid the other way round, transpose it
 * before pasting, and say so in the commit. */
static const signed char maze[][MAZE_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,0,1,1,1,1,1,1,1},
    {1,1,1,1,1,0,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,0,1,1,1,1,1,1,1},
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
    {1,1,1,1,1,1,1,0,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
    {1,1,1,1,1,1,1,0,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

#define MAZE_ROWS ((int)(sizeof maze / sizeof maze[0]))

/* Start, then the four waypoints, then the target: {row, column}, row 0 at the
 * top, column 0 at the left. Six entries, visited in this order.
 *
 * If staff give fewer or more waypoints, change MAZE_NPOINTS and the list to
 * match -- the planner walks whatever is here, in order, and nothing else
 * assumes six. */
/* TEST TOUR, not a staff map: ten waypoints chosen so the map above
 * exercises every digit 1-9, and reaches the north, east and west borders as
 * well as the south one it starts on. The comment on each says what the
 * robot does on arriving there. Two turns are NOT waypoints: (0,14), where
 * A* rounds the top-right corner, and (16,6), in and out of the spur.
 * Put the staff points back for the demo. */
#define MAZE_NPOINTS 12
static const signed char mazePoint[MAZE_NPOINTS][2] = {
    { 18,  0 },     /* start       -- bottom-left corner, facing east      */
    { 18,  9 },     /* waypoint 1  -- 9L  180 at the south border, swung   */
                    /*                through the north arm, not the edge */
    { 18,  4 },     /* waypoint 2  -- 6R  right off the south border       */
    {  0,  4 },     /* waypoint 3  -- 5R  meets the north border head-on   */
                    /*   (0,14)       4R  top-right corner, not a waypoint */
    {  4, 14 },     /* waypoint 4  -- 6R  right off the east border        */
    {  4,  0 },     /* waypoint 5  -- 9L  180 at the west border, nose in  */
                    /*                the dark, both sides open           */
    {  4,  6 },     /* waypoint 6  -- 6R  right, down column 6             */
                    /*   (16,6)       7L  into the spur, not a waypoint    */
    { 16,  7 },     /* waypoint 7  -- 2F  in the spur: no side lines       */
    { 16,  8 },     /* waypoint 8  -- 8L  dead end: 180 through bare floor */
                    /*   (16,6)       7L  out of the spur                  */
    { 18,  6 },     /* waypoint 9  -- 5L  meets the south border head-on   */
    { 18, 14 },     /* waypoint 10 -- 1L  bottom-right corner              */
    { 15, 14 }      /* TARGET      -- 3 blocks up the east border          */
};

/* Which way the robot faces while it sits on the start.
 * One of HDG_NORTH, HDG_EAST, HDG_SOUTH, HDG_WEST.
 * North is toward row 0, east is toward the last column. */
#define MAZE_START_HEADING  HDG_EAST

#endif /* MAZE_MAP_H */
