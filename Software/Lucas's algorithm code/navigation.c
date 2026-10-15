#include "navigation.h"
#include <stddef.h>

static bool Direction(BfsCell from, BfsCell to, NavHeading *heading)
{
    int32_t dr = (int32_t)to.row - from.row;
    int32_t dc = (int32_t)to.col - from.col;
    if (dr == -1 && dc == 0) *heading = NAV_NORTH;
    else if (dr == 0 && dc == 1) *heading = NAV_EAST;
    else if (dr == 1 && dc == 0) *heading = NAV_SOUTH;
    else if (dr == 0 && dc == -1) *heading = NAV_WEST;
    else return false;
    return true;
}

static bool Road(const Navigation *nav, int32_t row, int32_t col)
{
    return row >= 0 && col >= 0 && row < nav->rows && col < nav->cols &&
           nav->map[row * nav->cols + col] == 0;
}

/* Include junctions even when the chosen route continues straight. Corners
 * also count as landmarks. Ordinary straight corridor cells are skipped. */
static bool Landmark(const Navigation *nav, uint16_t index)
{
    BfsCell cell = nav->path[index];
    bool north = Road(nav, (int32_t)cell.row - 1, cell.col);
    bool east = Road(nav, cell.row, (int32_t)cell.col + 1);
    bool south = Road(nav, (int32_t)cell.row + 1, cell.col);
    bool west = Road(nav, cell.row, (int32_t)cell.col - 1);
    NavHeading incoming, outgoing;
    unsigned int degree = north + east + south + west;
    Direction(nav->path[index - 1], cell, &incoming);
    Direction(cell, nav->path[index + 1], &outgoing);
    return degree != 2 || incoming != outgoing;
}

static void SelectAction(Navigation *nav)
{
    unsigned int turn;
    if (nav->index == nav->length - 1) {
        nav->next_index = nav->index;
        nav->state = NAV_DONE;
        nav->action = NAV_STOP;
        return;
    }
    nav->next_index = nav->index + 1;
    while (nav->next_index < nav->length - 1 &&
           !Landmark(nav, nav->next_index)) ++nav->next_index;
    Direction(nav->path[nav->index], nav->path[nav->index + 1],
              &nav->desired_heading);
    turn = ((unsigned int)nav->desired_heading + 4U - nav->heading) % 4U;
    if (turn == 0) {
        nav->state = NAV_FOLLOWING;
        nav->action = NAV_FORWARD;
    } else {
        nav->state = NAV_TURN_REQUESTED;
        nav->action = turn == 1 ? NAV_RIGHT : (turn == 3 ? NAV_LEFT : NAV_UTURN);
    }
}

void NavigationFail(Navigation *nav)
{
    if (nav != NULL) {
        nav->state = NAV_FAULT;
        nav->action = NAV_STOP;
    }
}

bool NavigationStart(Navigation *nav, const uint8_t *map,
                     uint16_t rows, uint16_t cols, const BfsCell *path,
                     uint16_t length, NavHeading heading)
{
    uint16_t i;
    NavHeading direction;
    if (nav == NULL) return false;
    *nav = (Navigation){0};
    NavigationFail(nav);
    if (map == NULL || path == NULL || rows == 0 || cols == 0 ||
        (uint32_t)rows * cols > BFS_MAX_CELLS || length == 0 ||
        length > BFS_MAX_CELLS || (unsigned int)heading > NAV_WEST) return false;
    nav->map = map;
    nav->rows = rows;
    nav->cols = cols;
    for (i = 0; i < length; ++i) {
        if (!Road(nav, path[i].row, path[i].col) ||
            (i > 0 && !Direction(path[i - 1], path[i], &direction))) return false;
    }
    nav->path = path;
    nav->length = length;
    nav->heading = heading;
    SelectAction(nav);
    return true;
}

bool NavigationArrive(Navigation *nav, uint16_t index)
{
    if (nav == NULL || nav->state == NAV_IDLE || nav->state == NAV_FAULT)
        return false;
    if (index == nav->index) return true; /* Duplicate position report. */
    if (nav->state != NAV_FOLLOWING || index != nav->next_index) {
        NavigationFail(nav);
        return false;
    }
    nav->index = index;
    SelectAction(nav);
    return true;
}

bool NavigationTurnStarted(Navigation *nav)
{
    if (nav == NULL || nav->state != NAV_TURN_REQUESTED) return false;
    nav->state = NAV_TURNING;
    nav->action = NAV_WAIT;
    return true;
}

bool NavigationTurnFinished(Navigation *nav, bool success)
{
    if (nav == NULL || nav->state != NAV_TURNING) return false;
    if (!success) {
        NavigationFail(nav);
        return false;
    }
    nav->heading = nav->desired_heading;
    /* A turn changes heading, not position along the path. */
    nav->state = NAV_FOLLOWING;
    nav->action = NAV_FORWARD;
    return true;
}
