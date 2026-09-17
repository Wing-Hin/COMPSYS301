# heapq implements a min-priority queue. Unlike the FIFO queue used by BFS,
# A* removes the node with the smallest estimated total path cost first.
import heapq


def manhattan_distance(node, goal):
    """Estimate the remaining cost using horizontal and vertical distance."""

    # Split each (row, column) coordinate into its two components.
    row, col = node
    goal_row, goal_col = goal

    # Manhattan distance is the number of grid movements needed if there were
    # no walls. It is suitable because diagonal movement is not permitted.
    return abs(row - goal_row) + abs(col - goal_col)


def astar(grid, start, goal, explored_order=None):
    """
    Find a shortest path through a grid using the A* algorithm.

    grid contains 0 for a road and 1 for a wall.
    start and goal are (row, column) coordinate tuples.
    The returned path includes both start and goal.
    """

    # The caller can provide a list when it wants to record exploration order.
    # If no list is supplied, create a local one so the search still works.
    if explored_order is None:
        explored_order = []

    # An empty grid has no valid cells to search.
    if not grid or not grid[0]:
        return None

    # Store the grid dimensions so coordinates can be bounds-checked.
    rows = len(grid)
    cols = len(grid[0])

    # Return True only when a node is inside the grid and contains a road.
    def is_valid(node):
        row, col = node
        return (
            # Check bounds before accessing grid[row][col].
            0 <= row < rows
            and 0 <= col < cols
            # A value of 0 is a road. A value of 1 is a wall.
            and grid[row][col] == 0
        )

    # Searching cannot begin if either endpoint is outside the grid or a wall.
    if not is_valid(start) or not is_valid(goal):
        return None

    # Each heap entry is (f_score, g_score, node). heapq removes the entry
    # with the smallest f-score first. Here:
    #   g = actual movement cost from start
    #   h = estimated remaining cost to goal
    #   f = g + h
    start_g = 0
    start_f = start_g + manhattan_distance(start, goal)

    # open_list contains discovered nodes that may still need to be explored.
    # The start node is initially the only entry.
    open_list = [(start_f, start_g, start)]

    # Store the lowest known actual cost from start to every discovered node.
    # This also helps identify whether a node has been discovered previously.
    g_score = {start: 0}

    # Store how each node was reached on its cheapest known route. For example,
    # parent[(2, 3)] = (2, 2) means the route entered (2, 3) from (2, 2).
    parent = {}

    # Changes in row and column for the four permitted movements.
    directions = [
        (-1, 0),  # Up: decrease row
        (1, 0),   # Down: increase row
        (0, -1),  # Left: decrease column
        (0, 1),   # Right: increase column
    ]

    # Continue until the goal is found or there are no candidates remaining.
    while open_list:
        # Remove the node with the smallest f-score. BFS removes the oldest
        # node instead, whereas A* uses this calculated priority.
        current_f, current_g, current = heapq.heappop(open_list)

        # heapq cannot directly update an entry. If a cheaper route was found,
        # both entries may remain in the heap, so ignore the outdated one.
        if current_g != g_score[current]:
            continue

        # A node is explored when its best entry is removed from the heap.
        explored_order.append(current)

        # With the Manhattan heuristic, removing the goal here means its
        # shortest route has been found.
        if current == goal:
            # Parent links point backwards, so begin at the goal and repeatedly
            # move to the node used to reach the current node.
            path = []
            path_node = goal

            # Add every node except start while walking through the parents.
            while path_node != start:
                path.append(path_node)
                path_node = parent[path_node]

            # Include start, then reverse goal-to-start into start-to-goal.
            path.append(start)
            path.reverse()
            return path

        # Separate the coordinate so movement offsets can be applied.
        current_row, current_col = current

        # Generate each of the current node's four possible neighbours.
        for row_change, col_change in directions:
            neighbour = (
                current_row + row_change,
                current_col + col_change,
            )

            # Skip coordinates that are outside the maze or contain a wall.
            if not is_valid(neighbour):
                continue

            # Every valid movement costs 1, so this route to the neighbour
            # costs one more than the route to the current node.
            tentative_g = current_g + 1

            # Record the route if the neighbour is new or this route is cheaper
            # than the route that was previously recorded.
            if neighbour not in g_score or tentative_g < g_score[neighbour]:
                # Save the route information needed for path reconstruction.
                parent[neighbour] = current
                g_score[neighbour] = tentative_g

                # Calculate h and then f = g + h for this neighbour.
                h_score = manhattan_distance(neighbour, goal)
                neighbour_f = tentative_g + h_score

                # Add the neighbour to the priority queue. The heap will place
                # the smallest f-score at the front automatically.
                heapq.heappush(
                    open_list,
                    (neighbour_f, tentative_g, neighbour),
                )

    # The priority queue became empty without reaching the goal.
    return None


if __name__ == "__main__":
    # Example maze used to test the search. Each inner list is one grid row.
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

    # Coordinates use the form (row, column).
    start = (13, 0)
    goal = (0, 17)

    # Pass in this list so astar() can record the order of explored nodes.
    explored_order = []
    path = astar(grid, start, goal, explored_order)

    # A None result means the heap became empty before reaching the goal.
    if path is None:
        print("No path found")
    else:
        # The path includes start, so the number of movements is one less than
        # the number of coordinates in the path.
        print("Shortest path:", path)
        print("Number of moves:", len(path) - 1)

    # This makes it easy to compare A* exploration with BFS exploration.
    print("Explored order:", explored_order)
