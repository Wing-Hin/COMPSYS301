# Robot state decision

`robot_state.c/.h` classify one complete sensor reading using
`RobotDecideState(frame.state)`. They use the existing CS301 sensor definitions
and do not read hardware or command motors. The Python BFS/A* experiments remain
in the separate `Software/Lucas's algorithm code` folder.

| Sensor reading | Returned state |
| --- | --- |
| Any unknown/invalid reading, or NULL input | `ROBOT_STATE_SENSOR_FAULT` |
| Q3 and Q4 black | `ROBOT_STATE_JUNCTION` |
| Q3 black, Q4 white | `ROBOT_STATE_LEFT_BRANCH` |
| Q4 black, Q3 white | `ROBOT_STATE_RIGHT_BRANCH` |
| All six white | `ROBOT_STATE_LINE_LOST` |
| Otherwise (some middle/rear black, outer sensors white) | `ROBOT_STATE_FOLLOW_LINE` |

The mapping matches Turning: front left-to-right is Q3, Q6, Q5, Q4;
rear middle sensors are Q1 and Q2. Confirm this wiring on the robot.
Black is `SENSOR_BLACK` (0), not 1.

Branch states mean a **possible branch was detected**. A single reading cannot
prove whether it is a corner, a T junction, a crossing, a wide black marking,
or a large line-following error. It also cannot establish that a straight exit
exists. The route planner chooses whether to turn or continue; this function
does not automatically choose left at a junction or declare a finish line.

## Using it in the main program

Add `robot_state.c` and `robot_state.h` to the PSoC project, with this folder and
`CS301_Class.cydsn` in the compiler include paths. Do not add `tests/` to firmware.
There is no second `main()` and no change to the existing main program yet.

```c
#include "sensor.h"
#include "robot_state.h"

/* Inside the application's sensor-update service: */
SensorFrame frame = sensors_GetFrame();
if (frame.fresh) {
    RobotState detectedState = RobotDecideState(frame.state);
    /* Pass detectedState to your application's state machine. */
}
```

Read `sensors_GetFrame()` once per update and share that frame with other
controllers: reading it clears the driver's freshness flag. If Turning is
integrated, copy that same frame into `controlSensorFrame`.

This function reports the current sample, not a persistent manoeuvre state.
Before starting a turn, confirm the branch over consecutive **fresh** frames
(three is a starting point). Cached readings must not count as new evidence.
Handle invalid data immediately and track time since the last fresh frame to
stop on a sensor stall; this function has no clock and cannot detect staleness.
`LINE_LOST` and `SENSOR_FAULT` report conditions; main must actually stop motors.

Once main calls `TurnStart()`, keep the active turn state until `TurnUpdate()`
reports DONE or FAULT. Do not let this classifier or `straight()` replace an
active turn: white sensors are expected during rotation. Handle completion
and wait until the old branch is cleared before allowing another turn request.

## Host test

From the repository root on macOS/Linux:

```sh
build_dir=$(mktemp -d /tmp/robot-state-tests.XXXXXX)
cc -std=c99 -Wall -Wextra -Werror -pedantic \
  -ISoftware/Lucas/tests \
  -ISoftware/CS301_Class.cydsn \
  Software/Lucas/robot_state.c \
  Software/Lucas/tests/test_robot_state.c \
  -o "$build_dir/test_robot_state"
"$build_dir/test_robot_state"
```

Tests use the real `sensor.h` and a minimal host replacement for `project.h`.
They check all 64 black/white patterns and invalid inputs. They do not validate
physical sensor geometry or build the full PSoC firmware.
