from collections import deque


def bfs(grid, start, goal, explored_order=None):
    """Return the shortest path from start to goal, or None if none exists."""
    # The caller can provide a list when it wants to record exploration order.
    if explored_order is None:
        explored_order = []

    if not grid or not grid[0]:
        return None

    rows = len(grid)
    cols = len(grid[0])

    # A location is valid if it is inside the grid and is not a wall.
    def is_valid(location):
        row, col = location
        return (
            0 <= row < rows
            and 0 <= col < cols
            and grid[row][col] == 0
        )

    if not is_valid(start) or not is_valid(goal):
        return None

    # Queue for BFS.
    queue = deque([start])

    # Mark nodes as visited when they enter the queue. This prevents a node
    # from being added more than once.
    visited = {start}

    # parent[child] = node used to first reach child.
    parent = {}

    # Up, down, left, right.
    directions = [
        (-1, 0),
        (1, 0),
        (0, -1),
        (0, 1),
    ]

    while queue:
        current = queue.popleft()

        # A node is explored when it is removed from the queue.
        explored_order.append(current)

        if current == goal:
            break

        current_row, current_col = current

        for row_change, col_change in directions:
            neighbour = (
                current_row + row_change,
                current_col + col_change,
            )

            if is_valid(neighbour) and neighbour not in visited:
                visited.add(neighbour)
                parent[neighbour] = current
                queue.append(neighbour)

    if goal not in visited:
        return None

    # Follow the parent links backwards from the goal to the start.
    path = []
    current = goal

    while current != start:
        path.append(current)
        current = parent[current]

    path.append(start)
    path.reverse()
    return path


if __name__ == "__main__":
    grid = [
        [0, 0, 0, 0, 0],
        [0, 1, 1, 1, 0],
        [0, 0, 0, 1, 0],
        [1, 1, 0, 1, 0],
        [0, 0, 0, 0, 0],
    ]

    start = (0, 0)
    goal = (4, 4)
    explored_order = []
    path = bfs(grid, start, goal, explored_order)

    if path is None:
        print("No path found")
    else:
        print("Shortest path:")
        print(path)
        print("Number of moves:", len(path) - 1)

    print("Explored order:", explored_order)
