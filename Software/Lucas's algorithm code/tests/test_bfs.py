"""Host parity tests. Run with python3 tests/test_bfs.py; requires cc."""
import ctypes as C
from pathlib import Path
import random
import runpy
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
reference = runpy.run_path(str(ROOT / 'BFS_test.py'))['bfs']

class Cell(C.Structure):
    _fields_ = [('row', C.c_uint16), ('col', C.c_uint16)]

class Workspace(C.Structure):
    _fields_ = [('queue', C.c_uint16 * 512), ('parent', C.c_uint16 * 512)]

with tempfile.TemporaryDirectory() as temp:
    library = Path(temp) / 'bfs.so'
    subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                    '-shared', '-fPIC', str(ROOT / 'bfs.c'), '-o', str(library)], check=True)
    find = C.CDLL(str(library)).BfsFindPath
    find.argtypes = [C.POINTER(C.c_uint8), C.c_uint16, C.c_uint16,
                     Cell, Cell, C.POINTER(Workspace), C.POINTER(Cell),
                     C.c_uint16, C.POINTER(C.c_uint16)]
    find.restype = C.c_int

    def check(grid, start, goal, capacity=512):
        rows, cols = len(grid), len(grid[0]) if grid else 0
        flat = (C.c_uint8 * (rows * cols))(*(v for row in grid for v in row))
        workspace, path, length = Workspace(), (Cell * 512)(), C.c_uint16(999)
        status = find(flat, rows, cols, Cell(*start), Cell(*goal),
                      C.byref(workspace), path, capacity, C.byref(length))
        invalid = (not rows or not cols or rows * cols > 512 or
                   any(r >= rows or c >= cols or grid[r][c] != 0 for r, c in (start, goal)))
        expected = None if invalid else reference(grid, start, goal)
        if invalid:
            assert status == 2 and length.value == 0
        elif expected is None:
            assert status == 1 and length.value == 0
        elif len(expected) > capacity:
            assert status == 3 and length.value == len(expected)
        else:
            assert status == 0
            assert [(p.row, p.col) for p in path[:length.value]] == expected
        return length.value

    # Read the supplied example without printing its diagnostic output.
    import ast
    tree = ast.parse((ROOT / 'BFS_test.py').read_text())
    example = next(n for n in tree.body if isinstance(n, ast.If))
    grid = ast.literal_eval(next(n.value for n in example.body
                                if isinstance(n, ast.Assign) and n.targets[0].id == 'grid'))
    length = check(grid, (13, 0), (0, 17))
    check([[0]], (0, 0), (0, 0))
    check([[0, 1, 0]], (0, 0), (0, 2))
    check([[1]], (0, 0), (0, 0))
    check([[0]], (1, 0), (0, 0))
    check([], (0, 0), (0, 0))
    check([[0] * 513], (0, 0), (0, 512))
    check([[0] * 512], (0, 0), (0, 511))
    check([[0, 0]], (0, 0), (0, 1), capacity=1)
    check([[0]], (0, 0), (0, 0), capacity=0)
    rng = random.Random(301)
    for _ in range(1000):
        rows, cols = rng.randint(1, 20), rng.randint(1, 20)
        grid = [[int(rng.random() < .3) for _ in range(cols)] for _ in range(rows)]
        check(grid, (rng.randrange(rows), rng.randrange(cols)),
              (rng.randrange(rows), rng.randrange(cols)))
    ws, output, count = Workspace(), (Cell * 512)(), C.c_uint16(999)
    assert find(None, 1, 1, Cell(), Cell(), C.byref(ws), output, 512, C.byref(count)) == 2
    assert count.value == 0
    print(f'PASS: example ({length - 1} moves), edge cases, and 1000 seeded Python parity cases')
