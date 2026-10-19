/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#include <project.h>
#include <stdint.h>
#include "map.h"
#include "bfs.h"

typedef enum {
    LEFT_BRANCH,  /* Front-left outer sensor sees black. */
    RIGHT_BRANCH, /* Front-right outer sensor sees black. */
    T_JUNCTION,      /* Both front outer sensors see black. */
    CROSS_ROAD,
    LEFT_T_JUNCTION,
    RIGHT_T_JUNCTION,
} typeofJunctions;

typedef enum {
    LEFT,
    RIGHT,
    NO_TURN,
} Direction_Turned;

/* Grid position of a junction, same layout as BfsCell. */
typedef struct {
    uint16_t row;
    uint16_t col;
} JunctionCord;

/* One junction found on the planned path. */
typedef struct {
   uint8 numOfJunctionsIgnore;  /* junctions to drive straight through before this turn */
   float numOfNode;
   typeofJunctions type;
   JunctionCord cord;
   Direction_Turned turn;       /* which way to turn at this junction */
} Junctions;

/* A path can't have more junctions than the map has cells, so the list never
 * overflows. 285 x 16 bytes is about 4.6 KB of the PSoC 5LP's 64 KB SRAM. */
#define JUNCTIONS_MAX MAP_CELLS

/* Every junction where the robot turns, in the order it reaches them. */
typedef struct {
    Junctions items[JUNCTIONS_MAX];
    uint16_t count;  /* number of valid entries in items */
    uint8 numOfJunctionsIgnoreToGoal;  /* junctions to drive straight through after the last turn, before the goal */
    uint8 numOfNodesToGoal;
} JunctionList;

/* Records every junction on a BFS path where the robot turns. Pass the map and the
 * path/path_length from BfsFindPath. Returns NULL on invalid input.
 * The list is static: the next call overwrites it, so copy anything you need to keep. */
JunctionList *junctionsDetections(const uint8_t *map, const BfsCell *path,
                                  uint16_t pathLength);