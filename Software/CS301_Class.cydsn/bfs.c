#include "bfs.h"
#include <stddef.h>

#define BFS_UNVISITED UINT16_MAX

BfsStatus BfsFindPath(const uint8_t *grid, uint16_t rows, uint16_t cols,
                      BfsCell start, BfsCell goal, BfsWorkspace *workspace,
                      BfsCell *path, uint16_t path_capacity,
                      uint16_t *path_length)
{
    /* Match BFS_test.py's tie-breaking order: up, down, left, right. */
    static const int8_t dr[4] = {-1, 1, 0, 0};
    static const int8_t dc[4] = {0, 0, -1, 1};
    uint32_t cells = (uint32_t)rows * cols;
    uint16_t head = 0, tail = 0, i, start_id, goal_id, current;
    uint16_t length = 1;

    if (path_length == NULL) return BFS_INVALID_INPUT;
    *path_length = 0;
    if (grid == NULL || workspace == NULL || path == NULL ||
        cells == 0 || cells > BFS_MAX_CELLS ||
        start.row >= rows || start.col >= cols ||
        goal.row >= rows || goal.col >= cols) return BFS_INVALID_INPUT;

    start_id = (uint16_t)(start.row * cols + start.col);
    goal_id = (uint16_t)(goal.row * cols + goal.col);
    if (grid[start_id] != 1 || grid[goal_id] != 1) return BFS_INVALID_INPUT;

    for (i = 0; i < cells; ++i) workspace->parent[i] = BFS_UNVISITED;
    workspace->parent[start_id] = start_id;
    workspace->queue[tail++] = start_id;

    while (head < tail) {
        uint16_t direction;
        // queue.popleft (python)
        current = workspace->queue[head++];
        if (current == goal_id) break;
        for (direction = 0; direction < 4; ++direction) {
            int row = (int)(current / cols) + dr[direction];
            int col = (int)(current % cols) + dc[direction];
            uint16_t next;
            if (row < 0 || col < 0 || row >= rows || col >= cols) continue; // bound check
            next = (uint16_t)(row * cols + col);
            if (grid[next] != 1 || workspace->parent[next] != BFS_UNVISITED) continue;
            /* Mark on enqueue: each cell occupies the queue at most once. */
            workspace->parent[next] = current;
            workspace->queue[tail++] = next;
        }
    }
    if (workspace->parent[goal_id] == BFS_UNVISITED) return BFS_NO_PATH;

    for (current = goal_id; current != start_id;
         current = workspace->parent[current]) ++length;
    *path_length = length;
    if (length > path_capacity) return BFS_PATH_TOO_SMALL;

    current = goal_id;
    for (i = length; i > 0; --i) {
        path[i - 1].row = current / cols;
        path[i - 1].col = current % cols;
        current = workspace->parent[current];
    }
    return BFS_FOUND;
}
