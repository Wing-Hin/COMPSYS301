# Line follower host tests

Run from the repository root on macOS/Linux:

```sh
build_dir=$(mktemp -d /tmp/line-follow-tests.XXXXXX)
cc -std=c99 -Wall -Wextra -Werror \
  -ISoftware/CS301_Class.cydsn/tests/line_follow_stubs \
  -ISoftware/CS301_Class.cydsn \
  Software/CS301_Class.cydsn/tests/test_line_follow.c \
  Software/CS301_Class.cydsn/line_follow.c \
  -o "$build_dir/test_line_follow"
"$build_dir/test_line_follow"
```

Tests cover all 16 sensor patterns, input speeds 0..255, mirror symmetry,
invalid readings, motor enabling, the shared sensor frame, stale-data stopping,
recovery and clock wrap. Real main and follower code are compiled with mocked
hardware calls. Do not add these tests or stub headers to the firmware project.

The assumed mapping is Q6/Q5 front left/right and Q1/Q2 rear left/right.
Q3/Q4 do not steer. Tune gains 4/2 and the 50 ms sensor timeout in line_follow.h.
Main keeps its existing base speed 10; with one pair missing the base is halved.
Stops automatically recover when fresh valid readings arrive.

Main uses straightFromFrame; the compatibility straight() wrapper reads frames
itself and requires caller-managed freshness timeouts. This checkout's main
does not run the turning controller: future integration must call following
only when turning is idle and must hand back control with the motors stopped.

PSoC firmware compilation and physical calibration are separate from these tests.
