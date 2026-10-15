#ifndef BFS_H
#define BFS_H

#include <stdint.h>

/* Capacity includes walls. The Python example uses 15 * 19 = 285 cells. */
#define BFS_MAX_CELLS 512U

typedef struct {
    uint16_t row;
    uint16_t col;
} BfsCell;

typedef enum {
    BFS_FOUND,
    BFS_NO_PATH,
    BFS_INVALID_INPUT,
    BFS_PATH_TOO_SMALL
} BfsStatus;

/* Caller-owned scratch storage: keep static/global on the microcontroller. */
typedef struct {
    uint16_t queue[BFS_MAX_CELLS];
    uint16_t parent[BFS_MAX_CELLS];
} BfsWorkspace;

/* grid is a flat row-major array of rows*cols bytes: 0 road, nonzero wall.
 * path includes start and goal. On FOUND, path_length is the cell count;
 * on PATH_TOO_SMALL it is the required capacity; otherwise it is zero.
 * Output path is untouched on failure. Supply non-overlapping buffers.
 * No allocation, hardware access, or motor commands are performed.
 */
BfsStatus BfsFindPath(const uint8_t *grid, uint16_t rows, uint16_t cols,
                      BfsCell start, BfsCell goal, BfsWorkspace *workspace,
                      BfsCell *path, uint16_t path_capacity,
                      uint16_t *path_length);

#endif
