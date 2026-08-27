from collections import deque 
import heapq


class Algorithm:
    def __init__(self, grid, start, goal, explored_order=None):
        self.grid = grid
        self.start = start
        self.goal = goal
        self.explored_order = explored_order
    
    def bfs(self):
        """Return the shortest path from start to goal, or None if none exists."""
        # The caller can provide a list when it wants to record exploration order.
        if not self.grid or not self.grid[0]:
            return None

        rows = len(self.grid)
        cols = len(self.grid[0])

        # A location is valid if it is inside the grid and is not a wall.
        def is_valid(location):
            row, col = location
            return (
                0 <= row < rows
                and 0 <= col < cols
                and self.grid[row][col] == 0
            )

        if not is_valid(self.start) or not is_valid(self.goal):
            return None

        # Queue for BFS.
        queue = deque([self.start])

        # Mark nodes as visited when they enter the queue. This prevents a node
        # from being added more than once.
        visited = {self.start}

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
            self.explored_order.append(current)

            if current == self.goal:
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

        if self.goal not in visited:
            return None

        # Follow the parent links backwards from the goal to the start.
        path = []
        current = self.goal

        while current != self.start:
            path.append(current)
            current = parent[current]

        path.append(self.start)
        path.reverse()
        return path


    def manhattan_distance(self, node, goal):
        """Estimate the remaining distance using horizontal and vertical moves."""
        row, col = node
        goal_row, goal_col = goal
        return abs(row - goal_row) + abs(col - goal_col)


    def astar(self):
        """Return the shortest path, or None if the goal cannot be reached."""
        # The caller can provide a list when it wants to record exploration order.

        if not self.grid or not self.grid[0]:
            return None

        rows = len(self.grid)
        cols = len(self.grid[0])

        # A node is valid when it is inside the grid and is a road.
        def is_valid(node):
            row, col = node
            return (
                0 <= row < rows
                and 0 <= col < cols
                and self.grid[row][col] == 0
            )

        if not is_valid(self.start) or not is_valid(self.goal):
            return None

        # Each heap entry is (f_score, g_score, node). heapq removes the entry
        # with the smallest f-score first.
        start_g = 0
        start_f = start_g + self.manhattan_distance(self.start, self.goal)
        open_list = [(start_f, start_g, self.start)]

        # Lowest known cost from start to each discovered node.
        g_score = {self.start: 0}

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

            self.explored_order.append(current)

            if current == self.goal:
                # Reconstruct the route by following parent links backwards.
                path = []
                path_node = self.goal

                while path_node != self.start:
                    path.append(path_node)
                    path_node = parent[path_node]

                path.append(self.start)
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

                    h_score = self.manhattan_distance(neighbour, self.goal)
                    neighbour_f = tentative_g + h_score

                    heapq.heappush(
                        open_list,
                        (neighbour_f, tentative_g, neighbour),
                    )

        # The priority queue became empty without reaching the goal.
        return None