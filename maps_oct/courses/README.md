# Test courses

Each `*.maze_map.h` is a complete `maze_map.h` for the robot: the grid in the
robot's 19 x 15 frame, the waypoints, and the start heading. To drive one,
copy it over `ROBOT/psoc_code_base/CS301_Class.cydsn/maze_map.h` (in the
project revert directory), rebuild, and project the matching `.map`.

| Course | Projector map | Start | What it tests |
|---|---|---|---|
| `tour-deadend.maze_map.h` | `../tour-deadend.map` | (18,0) facing east | every digit 1-9 on the full lattice plus one dead-end spur; all four borders |
| `hell-turns.maze_map.h` | `../hell.map` | (18,0) bottom-left, facing east | 40 turns a grid point apart -- staircases, a square wave, a 1x1 loop, 180s mid-map and at the border -- between long straights on three borders |
| `hell-but-possible.maze_map.h` | `../hell but possible.map` | file's top-left corner (a dead end), facing along the top row | the hard one: a 146-cell snake with one junction; 98 turns in ~160 blocks, a 2F on a straight, 180s at a dead end, a bend and a straight |
| `map2.maze_map.h` | `../map2.map` | file's top-left corner, facing along the top row | a real maze with crossroads: 7L/7F/7R, four crossroads in a row three times, 180s at a crossroads, in a corridor and in a corner, a 2F; 90 blocks |
| `damiansecrettech.maze_map.h` | `../damiansecrettech.map` | file row 5, col 0 (a dead end on the left edge), facing down the left column | right-hand track only: dead-end 180, the 3-wide crossroads band, 180s at a crossroads and a T, a 180 on a 17-block bare straight; 83 blocks |
| `impossible.maze_map.h` | `../impossible.map` | file row 0, col 2 (a dead end on the top edge), facing down the file | the staircase: one 159-cell line, a bend at almost every cell, no junctions; two lone-straight 2Fs and two 180s at bends; 170 blocks, 147 turns |
| `test_map.maze_map.h` | `../test_map.map` | file's top-left corner, facing DOWN the left column | the demo-like maze (15 dead ends, 45 Ts, 2 crossroads): 7L/7F/7R, Ts from every side, bends, a 2F, four 180s; 100 blocks, 43 turns |
| `test_map-hard.maze_map.h` | `../test_map.map` | bottom-right as you face the mat, facing LEFT along the bottom row | the hardest found: 127 blocks, 60 turns, 180s at a dead end, a bend, in a corridor and at a T's side, 7L/7F/7R |
| `test_map-dense.maze_map.h` | `../test_map.map` | bottom-right as you face the mat, facing UP the right-hand column | turns packed close, then along the row-12 straight, a 180, and down column 14 to the bottom row: 107 blocks, 62 turns, five 180s |
| `map_1.maze_map.h` | `../map_1.map` | file's top-left corner, facing along the top row | every decision map_1 has: bends, T from each side, 2F in a corridor, 180s at a T side, corridor, dead end, T stem and bend |

The `.map` files are 15 x 19, a quarter turn from the robot's frame. For
map_1 the robot's (r, c) is file row c, file column 18 - r.
