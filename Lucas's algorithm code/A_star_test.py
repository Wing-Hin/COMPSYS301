import heapq


def manhattan_distance(node, goal):
    """Estimate the remaining distance using horizontal and vertical moves."""
    row, col = node
    goal_row, goal_col = goal
    return abs(row - goal_row) + abs(col - goal_col)


def astar(grid, start, goal, explored_order=None):
    """Return the shortest path, or None if the goal cannot be reached."""
    # The caller can provide a list when it wants to record exploration order.
    if explored_order is None:
        explored_order = []

    if not grid or not grid[0]:
        return None

    rows = len(grid)
    cols = len(grid[0])

    # A node is valid when it is inside the grid and is a road.
    def is_valid(node):
        row, col = node
        return (
            0 <= row < rows
            and 0 <= col < cols
            and grid[row][col] == 0
        )

    if not is_valid(start) or not is_valid(goal):
        return None

    # Each heap entry is (f_score, g_score, node). heapq removes the entry
    # with the smallest f-score first.
    start_g = 0
    start_f = start_g + manhattan_distance(start, goal)
    open_list = [(start_f, start_g, start)]

    # Lowest known cost from start to each discovered node.
    g_score = {start: 0}

    # parent[child] stores the node used to reach child on the best route.
    parent = {}

    # Up, down, left, right.
    directions = [
        (-1, 0),
        (1, 0),
        (0, -1),
        (0, 1),
    ]

    while open_list:
        current_f, current_g, current = heapq.heappop(open_list)

        # Ignore an older heap entry if a cheaper route was found later.
        if current_g != g_score[current]:
            continue

        explored_order.append(current)

        if current == goal:
            # Reconstruct the route by following parent links backwards.
            path = []
            path_node = goal

            while path_node != start:
                path.append(path_node)
                path_node = parent[path_node]

            path.append(start)
            path.reverse()
            return path

        current_row, current_col = current

        for row_change, col_change in directions:
            neighbour = (
                current_row + row_change,
                current_col + col_change,
            )

            if not is_valid(neighbour):
                continue

            # Every valid movement has a cost of 1.
            tentative_g = current_g + 1

            # Update this node only if the new route is cheaper.
            if neighbour not in g_score or tentative_g < g_score[neighbour]:
                parent[neighbour] = current
                g_score[neighbour] = tentative_g

                h_score = manhattan_distance(neighbour, goal)
                neighbour_f = tentative_g + h_score

                heapq.heappush(
                    open_list,
                    (neighbour_f, tentative_g, neighbour),
                )

    # The priority queue became empty without reaching the goal.
    return None


if __name__ == "__main__":
    grid = [
        [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1],
        [1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1],
        [1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1],
        [1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1],
        [1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1],
        [1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1],
        [1,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,1,0,1],
        [1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1],
        [1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,0,1,0,1],
        [1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1],
        [1,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1],
        [1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1],
        [1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1],
        [0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1],
        [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
    ]

    start = (13, 0)
    goal = (0, 17)

    explored_order = []
    path = astar(grid, start, goal, explored_order)

    if path is None:
        print("No path found")
    else:
        print("Shortest path:", path)
        print("Number of moves:", len(path) - 1)

    print("Explored order:", explored_order)
