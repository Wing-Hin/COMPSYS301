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
#include "branchSearch.h"
#include <stddef.h>

/* Returns 1 if (row, col) is inside the map and is road. */
static uint8_t isRoad(const uint8_t *map, int row, int col)
{
    if (row < 0 || col < 0 || row >= (int)MAP_ROWS || col >= (int)MAP_COLS) return 0; // bound check
    /* map is flat row-major: 0 is road, nonzero is wall. */
    return map[row * MAP_COLS + col] == 1;
}

/* Works out what the robot sees at cell when it arrives from prev.
 * Forward/left/right are relative to the robot's heading, not the map, so the
 * same cell can be a different junction type depending on the travel direction.
 * Returns 1 and sets *type for a junction, or 0 for a straight or a dead end. */
static uint8_t getJunctionType(const uint8_t *map, BfsCell prev, BfsCell cell, typeofJunctions *type){
    /* Heading = the step from prev to cell, each part is -1, 0 or 1.
     * int, not uint16_t, so a step up or left gives -1. */
    int hr = (int)cell.row - (int)prev.row;
    int hc = (int)cell.col - (int)prev.col;
    int r = (int)cell.row;
    int c = (int)cell.col;

    /* Rotating the heading 90 degrees gives left (-hc, hr) and right (hc, -hr).
     * e.g. heading up (-1, 0): left is (0, -1), right is (0, 1). */
    uint8_t forward = isRoad(map, r + hr, c + hc);
    uint8_t left    = isRoad(map, r - hc, c + hr);
    uint8_t right   = isRoad(map, r + hc, c - hr);

    if (forward && left && right)  *type = CROSS_ROAD;
    else if (left && right)        *type = T_JUNCTION;       /* no forward */
    else if (forward && left)      *type = LEFT_T_JUNCTION;
    else if (forward && right)     *type = RIGHT_T_JUNCTION;
    else if (left)                 *type = LEFT_BRANCH;      /* 90 degree left corner */
    else if (right)                *type = RIGHT_BRANCH;     /* 90 degree right corner */
    else return 0;                 /* forward only (straight) or dead end */
    return 1;
}

/* Works out which way the robot turns at cell, going prev -> cell -> next.
 * Returns 1 and sets *dir for a left or right turn, or 0 if it goes straight.
 * BFS never doubles back, so a U-turn can't happen. */
static uint8_t getTurnDirection(BfsCell prev, BfsCell cell, BfsCell next, Direction_Turned *dir)
{
    /* Heading in (prev -> cell) and heading out (cell -> next). */
    int hr = (int)cell.row - (int)prev.row;
    int hc = (int)cell.col - (int)prev.col;
    int nr = (int)next.row - (int)cell.row;
    int nc = (int)next.col - (int)cell.col;

    /* Same left (-hc, hr) / right (hc, -hr) rotation as getJunctionType. */
    if (nr == -hc && nc == hr)      *dir = LEFT;
    else if (nr == hc && nc == -hr) *dir = RIGHT;
    else return 0;                  /* heading unchanged: straight */
    return 1;
}

/* Walks every cell of a BFS path, start to goal, and records each junction
 * where the robot has to turn. Junctions the robot drives straight through
 * are not recorded; they are counted into the next turn's numOfJunctionsIgnore.
 * map is the same grid given to BfsFindPath, path is its output and
 * pathLength is its path_length. */
JunctionList *junctionsDetections(const uint8_t *map, const BfsCell *path, uint16_t pathLength){
    /* static so the list outlives this call and stays off the small stack. */
    static JunctionList list;
    uint16_t i;
    typeofJunctions type;
    Direction_Turned dir;
    uint8 leftIgnored = 0;  /* straight-through junctions since the last turn */
    uint8 rightIgnored = 0;
    // validate the map and path.
    if (map == NULL || path == NULL) return NULL;

    list.count = 0;

    /* Start at 1: the start cell has no previous cell, so no heading to
     * judge left/right from. Stop before the goal: it has no next cell, so
     * the robot can't turn there. */
    for (i = 1; i + 1 < pathLength && list.count < JUNCTIONS_MAX; ++i) {
        if (!getJunctionType(map, path[i - 1], path[i], &type)) continue;

        if (getTurnDirection(path[i - 1], path[i], path[i + 1], &dir)) {
            Junctions *j = &list.items[list.count++];
            j->type = type;
            j->turn = dir;
            j->cord.row = path[i].row;
            j->cord.col = path[i].col;
            if (dir == LEFT){
                j->numOfJunctionsIgnore = leftIgnored;
            }
            else if(dir == RIGHT){
                j->numOfJunctionsIgnore = rightIgnored;
            }
            leftIgnored = 0;
            rightIgnored = 0;  /* start counting again for the next turn */
        } else {
            if(type == CROSS_ROAD || type == T_JUNCTION){
                leftIgnored++;
                rightIgnored++;
            }
            else if(type == LEFT_T_JUNCTION || type == LEFT_BRANCH){
                leftIgnored++;
            }
            else if (type == RIGHT_T_JUNCTION || type == RIGHT_BRANCH){
                rightIgnored++;
            }
        }
    }

    /* Junctions passed after the last turn have no turn to attach to, so
     * keep them here for the run to the goal. With no turns at all, this is
     * every junction on the path. */
    list.numOfJunctionsIgnoreToGoal = leftIgnored + rightIgnored;

    return &list;
}

/* [] END OF FILE */
