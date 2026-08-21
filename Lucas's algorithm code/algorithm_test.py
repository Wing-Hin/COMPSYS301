import argparse
import json
import sys

from algo import Algorithm


def load_map(mapfile):
    """Read a map file of digit rows (e.g. map_8.txt) into a 2D list of ints."""
    grid = []
    with open(mapfile) as f:
        for line in f:
            line = line.strip()
            if line:
                grid.append([int(ch) for ch in line])
    return grid


def main():
    parser = argparse.ArgumentParser(description="Run a path-finding algorithm on a map file.")
    parser.add_argument("mapfile", help="path to the map text file, e.g. map_8.txt")
    parser.add_argument("start_row", type=int, help="0-based start row")
    parser.add_argument("start_col", type=int, help="0-based start column")
    parser.add_argument("goal_row", type=int, help="0-based goal row")
    parser.add_argument("goal_col", type=int, help="0-based goal column")
    parser.add_argument("--algorithm", choices=["bfs", "astar"], default="bfs",
                        help="which algorithm to run (default: bfs)")
    parser.add_argument("--pretty", action="store_true",
                        help="indent the JSON output for human reading")
    args = parser.parse_args()

    grid = load_map(args.mapfile)
    start = (args.start_row, args.start_col)
    goal = (args.goal_row, args.goal_col)

    explored_order = []
    algorithm = Algorithm(grid, start, goal, explored_order)

    if args.algorithm == "bfs":
        path = algorithm.bfs()
    else:
        path = algorithm.astar()

    result = {
        "found": path is not None,
        "map": grid,
        "path": [list(node) for node in path] if path else [],
        "explored": [list(node) for node in explored_order],
        "moves": len(path) - 1 if path else -1,
    }

    print(json.dumps(result, indent=2 if args.pretty else None))
    return 0


if __name__ == "__main__":
    sys.exit(main())
