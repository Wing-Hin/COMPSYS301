# deque provides an efficient first-in, first-out queue. BFS always removes
# the node that has been waiting in the queue the longest.
from collections import deque


def bfs(grid, start, goal, explored_order=None):
    """
    Find a shortest path through a grid using Breadth-First Search.

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

    # Return True only when a location is inside the grid and contains a road.
    def is_valid(location):
        row, col = location
        return (
            # Check the row and column before accessing grid[row][col].
            0 <= row < rows
            and 0 <= col < cols
            # A value of 0 is a road. A value of 1 is a wall.
            and grid[row][col] == 0
        )

    # Searching cannot begin if either endpoint is outside the grid or a wall.
    if not is_valid(start) or not is_valid(goal):
        return None

    # The BFS queue begins with the start node. deque.popleft() below makes
    # this a first-in, first-out queue, so nodes are explored by distance.
    queue = deque([start])

    # Mark nodes as visited when they enter the queue. This prevents a node
    # from being added more than once through different neighbours.
    visited = {start}

    # Store how each node was first reached. For example, if (2, 3) was
    # reached from (2, 2), parent[(2, 3)] will contain (2, 2).
    parent = {}

    # Changes in row and column for the four permitted movements.
    # The list order is also the neighbour checking priority.
    directions = [
        (-1, 0),  # Up: decrease row
        (1, 0),   # Down: increase row
        (0, -1),  # Left: decrease column
        (0, 1),   # Right: increase column
    ]

    # Continue until the goal is found or there are no reachable nodes left.
    while queue:
        # Remove the oldest queued node. This is what gives BFS its
        # level-by-level exploration and shortest-path guarantee.
        current = queue.popleft()

        # A node is explored when it is removed from the queue.
        explored_order.append(current)

        # The first time BFS removes the goal, the shortest route is known.
        if current == goal:
            break

        # Separate the coordinate so movement offsets can be applied.
        current_row, current_col = current

        # Generate each of the current node's four possible neighbours.
        for row_change, col_change in directions:
            neighbour = (
                current_row + row_change,
                current_col + col_change,
            )

            # A neighbour can be added only if it is a road inside the grid
            # and has not already been discovered.
            if is_valid(neighbour) and neighbour not in visited:
                # Mark it immediately, before another node can add it again.
                visited.add(neighbour)

                # Remember the previous node for later path reconstruction.
                parent[neighbour] = current

                # Add the neighbour to the end of the BFS queue.
                queue.append(neighbour)

    # If the goal was never discovered, walls separate it from the start.
    if goal not in visited:
        return None

    # Reconstruct the shortest path. Parent links point backwards, so begin at
    # the goal and repeatedly move to the node that was used to reach it.
    path = []
    current = goal

    # Add every node except start while walking backwards through the parents.
    while current != start:
        path.append(current)
        current = parent[current]

    # Include the start node, then reverse goal-to-start into start-to-goal.
    path.append(start)
    path.reverse()
    return path


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

    # Pass in this list so bfs() can record the order of explored nodes.
    explored_order = []
    path = bfs(grid, start, goal, explored_order)

    # A None result means the queue became empty before reaching the goal.
    if path is None:
        print("No path found")
    else:
        # The path includes start, so the number of movements is one less than
        # the number of coordinates in the path.
        print("Shortest path:")
        print(path)
        print("Number of moves:", len(path) - 1)

    # This is useful for comparing BFS exploration with A* exploration.
    print("Explored order:", explored_order)
