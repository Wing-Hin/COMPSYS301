#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <stdbool.h>
#include "bfs.h"

/* Clockwise order makes relative turns a simple subtraction modulo four. */
typedef enum { NAV_NORTH, NAV_EAST, NAV_SOUTH, NAV_WEST } NavHeading;
typedef enum {
    NAV_STOP, NAV_FORWARD, NAV_LEFT, NAV_RIGHT, NAV_UTURN, NAV_WAIT
} NavAction;
typedef enum {
    NAV_IDLE, NAV_FOLLOWING, NAV_TURN_REQUESTED, NAV_TURNING,
    NAV_DONE, NAV_FAULT
} NavState;

typedef struct {
    const uint8_t *map;
    const BfsCell *path;
    uint16_t rows, cols, length;
    uint16_t index;       /* Last confirmed position in the BFS path. */
    uint16_t next_index;  /* Next corner, junction, or destination. */
    NavHeading heading;
    NavHeading desired_heading;
    NavState state;
    NavAction action;
} Navigation;

/* path and map must remain alive and unchanged throughout navigation.
 * Start at path[0] with a known heading. Validates every path step and cell.
 * Call only while stopped. Zero-initialize Navigation before its first use.
 */
bool NavigationStart(Navigation *nav, const uint8_t *map,
                     uint16_t rows, uint16_t cols, const BfsCell *path,
                     uint16_t length, NavHeading heading);
/* Confirm arrival at next_index, NOT every sensor reading of an intersection.
 * Repeating the current index is harmless. An unexpected index faults.
 * Passing straight through a junction advances the target without turning.
 */
bool NavigationArrive(Navigation *nav, uint16_t index);
bool NavigationTurnStarted(Navigation *nav);
bool NavigationTurnFinished(Navigation *nav, bool success);
void NavigationFail(Navigation *nav);

#endif
