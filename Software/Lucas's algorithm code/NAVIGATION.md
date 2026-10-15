# Following a BFS route

The firmware now has a route-navigation mode, enabled by default in
`Software/CS301_Class.cydsn/robot_navigation_config.h`.
It stays stopped with `ROBOT_NAV_CONFIG` until the blocked placeholder map is
replaced with a valid route configuration. Grid spacing is initially set to
100 encoder counts for the measured 9 cm centre-to-centre step; verify this
estimate on the robot. There is no assumed physical route.
Set `ROBOT_USE_ROUTE_NAVIGATION` to 0 to select the old branch-triggered mode.

## Files

- `navigation.c/.h`: portable decision logic, using a BFS coordinate path.
  Tracks heading, confirmed position, and next landmark. It requests straight,
  left, right or U-turn, waits for turn completion, and stops at the goal.
- `../CS301_Class.cydsn/robot_navigation.c/.h`: robot adapter. Runs BFS at
  initialization; combines encoder travel and sensor readings to confirm
  landmarks; calls the existing line follower and turning controller.
- `../CS301_Class.cydsn/robot_navigation_config.c/.h`: your map, endpoints,
  initial heading and calibration settings.
- `tests/test_navigation.py`: host tests for the decision logic, mocked robot
  integration, real turn-abort behaviour and cached sensor hardware adapter.

The source/header entries are added to `CS301_Class.cyprj`. Main calls
`RobotNavigationInit()` once and `RobotNavigationUpdate()` each loop while
continuing the motor speed regulator. In route mode it does not run the legacy
branch-turn or turn-update blocks. The turning adapter reads the single cached
sensor frame, and `TurnAbort()` allows sensor faults to stop an active turn.
The duplicate project-local turn headers forward to the canonical turning
folder so the controller and its caller use the same settings.

## Enter your route

Edit `robot_navigation_config.c`:

1. Replace `routeMap` with your flattened map (rows one after another): 0 road,
   1 wall. Set rows and columns to match the exact number of array entries.
2. Enter start and goal as `{row, column}`. Both must be roads.
3. Set the robot's actual initial heading: north means decreasing row, east
   increasing column, south increasing row, west decreasing column.
4. Place the robot at the specified start and point it along the first BFS
   path step. Inspect `RobotNavigationGet()->path` to see that step. Initial
   misalignment stops with `ROBOT_NAV_INITIAL_HEADING`, rather than using the
   junction turn's forward approach from an arbitrary starting pose.
5. Verify `cell_counts = 100` against measured forward encoder counts per grid
   step. This estimate uses your 9 cm centre-to-centre spacing, not the 10.5 cm
   outer-to-outer measurement. Initial estimate:

   `counts = cell_distance / (pi * wheel_diameter) * encoder_counts_per_rev`

   Use consistent distance units. The existing motor code assumes 228 counts
   per wheel revolution and a 65 mm wheel diameter; verify against your robot.
6. Calibrate `arrival_window`, encoder signs, speed, segment timeout and
   `turn_exit_offset` at low speed. The window must be less than half a cell.

The map is compiled into the firmware. There is no USB map-entry protocol in
this implementation. Change the configuration and rebuild for another route.

## How progress works

A grid cell is not necessarily an intersection. Navigation examines the map
and skips ordinary straight corridor cells, targeting the next corner,
junction (including unused side branches), or destination instead.

For each straight segment, expected counts are:

`(next_index - index) * cell_counts`

Signed encoder deltas track both wheels independently, including 16-bit wrap.
Reverse movement subtracts progress. Each wheel must be within the configured
position window before an intersection can be accepted. A side sensor must
show a branch on three consecutive fresh frames. Three branch-free frames
rearm detection after leaving a landmark. Repeated readings of one junction
therefore do not advance the route again.

Once a landmark is confirmed, the route decides the movement. A left branch
does not force a left turn: if BFS goes straight, line following continues.
For a turn, the requested side must be detected. Heading changes only after
`TURN_DONE`; no line-follow commands are issued while the turn owns the motors.
The final goal uses encoder distance, so it may lie inside a straight corridor.
Both wheels must reach it before the robot stops.

## Physical calibration assumptions

This tracker assumes an axis-aligned, equal-spacing grid of projected lines.
A corner or junction in the map must produce a detectable outer-sensor branch.
It does not perform general localisation or recover from a wrong map or a
missed junction. A missed landmark, stale/invalid sensors, stalled segment or
failed turn causes a latched stop; inspect `RobotNavigationGetError()` in the
debugger. Reinitialize only after fixing the cause and restoring the start pose.

Use a consistent physical reference for cell positions, such as the calibrated
front-sensor landmark trigger. Turning includes forward approach and real
motion: `turn_exit_offset` represents how far into the new straight segment
the robot has already travelled when the turn finishes, relative to that same
reference. It can be negative. A single offset assumes repeatable left/right
turn geometry; revise the localisation if your measurements need separate
corrections. A zero offset is a placeholder, not a measured value. Tune the
arrival window to include detection/debounce delay without reaching a nearby
landmark. Goal stopping distance also needs physical verification.

The portable controller represents U-turns, but the current hardware turn
controller only supports left/right junction turns. The adapter stops on a
U-turn request. A shortest BFS path has no internal immediate reversal; the
initial-heading requirement prevents a starting U-turn. Do not substitute two
junction turns for an in-place U-turn: each existing turn includes an approach.

## Tests

From the repository root, on macOS/Linux with Python 3 and a C compiler:

```sh
python3 "Software/Lucas's algorithm code/tests/test_navigation.py"
python3 "Software/Lucas's algorithm code/tests/test_bfs.py"
```

The navigation runner uses address/undefined-behaviour sanitizers and warnings
as errors (with a documented exception for an existing unused turn helper).
Tests include all 16 heading combinations, BFS-to-navigation, corridor skipping,
straight-through junctions, duplicate events, left/right turn ownership, turn
completion/failure, destination stopping, bad configuration, wrong/missed
branches, stalled wheel, sensor failure and clock/encoder wrap. Both main-loop
modes receive host syntax checks. These checks do not replace a PSoC Creator
build or track testing. All files under `tests/` stay out of the firmware.
