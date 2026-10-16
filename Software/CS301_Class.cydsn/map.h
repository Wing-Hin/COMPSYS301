#ifndef MAP_H
#define MAP_H

#include <stdint.h>

/* All maps are 15 rows x 19 columns, flat row-major: 0 road, 1 wall.
 * Pass maps[id] to BfsFindPath as grid, with MAP_ROWS and MAP_COLS. */
#define MAP_ROWS  15U
#define MAP_COLS  19U
#define MAP_CELLS (MAP_ROWS * MAP_COLS)

typedef enum {
    MAP_DEBUG_COPY,          /* debug (Copy).map */
    MAP_DEBUG,               /* debug.map */
    MAP_HELL_BUT_POSSIBLE,   /* hell_but_possible.map */
    MAP_HELL,                /* hell.map */
    MAP_IMPOSSIBLE,          /* impossible.map */
    MAP_1_COPY,              /* map_1 (Copy).map */
    MAP_1,                   /* map_1.map */
    MAP_2,                   /* map2.map */
    MAP_TEST,                /* test_map.map */
    MAP_TOUR_DEADEND,        /* tour-deadend.map */
    MAP_TWO_LOOPS,           /* two-loops.map */
    MAP_DAMIANSECRETTECH,    /* damiansecrettech.map */
    MAP_67,                  /* 67.map */
    MAP_COUNT
} MapId;

extern const uint8_t *const maps[MAP_COUNT];
extern const char *const mapNames[MAP_COUNT];

#endif
