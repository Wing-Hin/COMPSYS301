# Left/right turning module: six sensors and encoder approach

`turn.c` and `turn.h` implement only the requested non-blocking turn sequence:
`IDLE -> APPROACH -> LEAVE_LINE -> FIND_LINE -> DONE`, with a total timeout to
`FAULT`. Add these files and `turn_config.h` to `Software/CS301_Class.cydsn` in PSoC Creator, plus a
completed copy of `turn_hardware.c.example`. The example is intentionally excluded
from builds until the hardware placeholders are completed. No generated project
files or existing ADC/normal-driving code have been changed.

This version uses the corrected layout: four sensors at the front and two middle
sensors at the back. APPROACH ends after measured forward encoder travel, as
selected for this layout. It does not wait for nonexistent rear outer sensors.

## Existing hardware mapping

The supplied `doc/psoc-breakout-interconnect-sheet.png` shows **six**, not eight,
sensor chains. Combined with
`doc/Phototransistor array layout on the sensor PCB (top view, dimensions in mm).png`,
the known mapping is:

| Position | Phototransistor | Existing sensor.h index | PSoC input |
| --- | --- | --- | --- |
| FL | Q3 | 2 | P0[7] |
| FM_L | Q6 | 5 | P1[5] |
| FM_R | Q5 | 4 | P1[4] |
| FR | Q4 | 3 | P1[2] |
| BM_L | Q1 | 0 | P0[5] |
| BM_R | Q2 | 1 | P0[6] |

Indices are the existing software's Q1..Q6 definitions; verify the ADC sequencer
channel order against the actual Creator schematic. The drawings do not establish
the wheel axle location. Q1/Q2 already sit on the original line when centred;
their black readings cannot identify axle arrival at a branch. No extra sensors
or ADC channels are needed for this version.

The hardware connections are:

- `ReadSensor`: the template reuses `sensors_GetFrame()` from `sensor.c` and
  converts its `SENSOR_BLACK == 0` convention to the turning module's black=1.
  Run `sensor_init()` once. The binary interface requires valid input: exclude
  `SENSOR_UNKNOWN` and stale frames before using it; neither means background.
  The existing `getSingleSensorState()` has an off-by-one bounds check, so the
  template uses the frame API. The driver comments describe an 8 ms frame window;
  5 ms reads can repeat a frame. Tune filtering against the actual frame rate,
  or update acquisition to provide a fresh frame each tick. Repeated observations
  are not independent ADC samples.
- `SetMotorSpeed`: the template reuses `MotorEnable`, `MotorDisable`,
  `MotorLeft_setDirection`, `MotorRight_setDirection`, and the existing
  `PWM_1`/`PWM_2` APIs. Sign selects direction; magnitude scales PWM duty. Zero
  clears PWM and disables motors. Verify stop/brake behavior and reversal timing.
  `MotorLeft_setRPM`/`MotorRight_setRPM` set regulator targets, not PWM duty;
  `Motor_maintainSpeed` can retain nonzero PWM at zero target, so they cannot
  directly implement the requested immediate signed-duty/stop contract.
- `ReadWheelEncoderCounts`: the additional encoder input uses the existing
  `QuadDec_M1_GetCounter()` and `QuadDec_M2_GetCounter()` APIs. Start both
  quadrature components once during startup. These are currently 16-bit x4
  counters. The module takes a baseline at TurnStart without resetting shared
  hardware counters, then accumulates signed deltas with wraparound correction.
  Each wheel must move fewer than 32768 counts between updates. No other code
  may reset either counter during APPROACH. The current
  `Motor_getDistanceTravelled()` is unsuitable: its source adds absolute counts
  repeatedly and divides integers before converting to distance. Reading the
  existing counters directly avoids those errors without changing that driver.

`TURN_APPROACH_COUNTS` is the forward distance required **per wheel** after the
main controller requests a turn. Both wheels must meet it for
`TURN_CONFIRM_READINGS` consecutive updates. Reverse travel subtracts; one wheel
cannot compensate for the other being stationary. Approach still commands equal
duties, so matched forward motion is an assumption, not an implemented steering
controller. Confirmation adds a small amount of forward travel before rotation.

## Controller handoff

`main_integration.c.example` contains a commented `MainControlUpdate5ms()` example
to adapt into main.c. It confirms a front intersection, requests the planned
left/right turn once, gates normal motor control, acknowledges DONE, and leaves
FAULT stopped for explicit recovery. It rearms junction detection only after
both outer sensors remain clear while driving normally. Its route, line-follow,
and completion hooks are application placeholders, not existing functions.
The current ADC/USB test main is not replaced automatically. Follow the startup
and timing notes in the example; the existing blocking loop is not a 5 ms scheduler.

1. Run the existing sensor acquisition continuously. At a newly detected front
   intersection, call `TurnStart(TURN_LEFT)` or `TurnStart(TURN_RIGHT)` once.
   A successful call records encoder baselines and commands forward approach.
   Invalid direction,
   busy state, and unhandled DONE/FAULT all reject a new request without effects.
2. Call `TurnUpdate()` once every **5 ms**. With the supplied adapter, wrap it in
   a short `CyEnterCriticalSection()` / `CyExitCriticalSection()` pair to keep both
   front middle reads in the same ADC frame. Do not wait for ADC conversions.
   Call Start/Update/Reset from one execution context; these functions are not
   reentrant.
3. Run normal driving **only when `TurnGetState() == TURN_IDLE`**, checking again
   after a successful TurnStart. This applies to every motor writer, including
   `Motor_maintainSpeed`, manual USB commands, and any motor-writing ISR. Merely
   calling TurnUpdate last is insufficient. The straight-line source in
   `Software/Abdur Code/line_follow.c` is a syntactically incomplete draft, so
   it cannot currently be called as a working function.
4. DONE and FAULT command `(0, 0)` on every update and remain latched. Handle the
   result, then call `TurnReset()` to release control. Reset is ignored while
   busy and does nothing in IDLE. Acknowledge FAULT only when ready to recover.
   The main controller must latch each junction to avoid requesting another
   turn while the same front intersection indication remains asserted.

The module cannot prevent an unrelated function from writing motor registers;
the controller's ownership guard is required. Its only hardware dependencies are
the sensor, motor and encoder functions, so timeout is measured by 5 ms calls rather than an
independent clock. A stalled scheduler also stalls timeout detection.

## Geometry assumptions and limits

The intended branch is approximately perpendicular to the approach line. At
rotation start, the wheel midpoint must be close enough to the intersection
centre and the robot centred on the incoming line. Calibrate forward travel from
the actual front-intersection trigger to that pivot position. Front-sensor
debounce, line width, request latency and approach confirmation affect the
required distance. Wheel slip and unequal wheel speeds also move the pivot,
even on a symmetric chassis. Encoders measure wheel rotation, not floor travel.

Both front middle sensors must initially fit inside the incoming black line,
then both be over background together for the confirmation count, and finally
both fit on the requested branch for the confirmation count. The drawing's
middle spacing is 10.16 mm, so the effective black width must accommodate that
spacing with margin for sensor footprints and placement error. A broad filled
junction or a front row too close to the pivot can eliminate the clear gap.

For an ideal centred pivot, point sensors at `(x,y)=(+/-a,d)`, and equal-width
perpendicular lines of half-width `h`, a sufficient geometric condition is
`a <= h < (d-a)/sqrt(2)`: both sensors are on the line at 0 and 90 degrees and
clear of both lines at 45 degrees. This is an ideal geometric check, not a
measured property of this robot. The actual axle offset and line width are not
provided, and the clear/black windows must also last long enough to be sampled.

The first sustained black pair after the clear gap is assumed to be the intended
branch. A missing branch, another marking, or a sufficiently long dropout of the
original line violates that assumption. If the branch is missed, a long rotation
can reacquire the original line; two binary sensors cannot identify which line
they see. Choose speed and timeout so a missed branch faults before that can
happen, or use heading/encoder limits if that guarantee is required. Three-sample
debounce rejects shorter observation glitches, not arbitrarily long ones.

DONE means confirmed line overlap, **not exact 90-degree alignment**. Sensor width,
debounce travel and coast after the stop command create angular error. Use a low
turning speed and validate both directions physically. The existing normal
controller should take over only after the main controller acknowledges DONE.

## Configuration and simulated verification

All commonly changed settings are grouped in **`turn_config.h`**. Change the
numbers there; `turn.h` includes it and validates the values at compile time.
Keep `TURN_UPDATE_MS` at 5 to match the scheduler contract.

Defaults: approach 20%, rotation 25%, forward travel 34 encoder counts
per wheel, three consecutive confirmations per transition, and 4000 ms for the
entire manoeuvre. **34 counts is an uncalibrated initial estimate**, about 30 mm
using the existing motor code's 57*4 counts per revolution and 65 mm wheel diameter:

```text
approach_counts = distance_mm * counts_per_wheel_revolution / (pi * wheel_diameter_mm)
```

Confirm the actual wheel counts per revolution, wheel diameter and front-sensor
to axle distance. The drawing's 29.97 mm row spacing is not proof of axle position.
Tune the threshold for the actual junction trigger and confirmation travel.
Set `TURN_LEFT_ENCODER_FORWARD_SIGN` and `TURN_RIGHT_ENCODER_FORWARD_SIGN` to +1
or -1 independently so forward movement accumulates positive counts. Both default
to +1 as placeholders; motor polarity does not establish encoder polarity.

At 5 ms per call, three readings
span 10 ms between the first and last observation, with a further sampling-phase
delay of up to 5 ms after a physical edge. A failed reading resets confirmation;
counts never carry between states. The timeout wins on the deadline tick.

On Windows with Visual Studio C tools installed, run from the repository root:

```bat
Software\Turning\tests\run_tests.cmd
```

The runner compile-checks the main integration example, builds native C with
`/std:c11 /W4 /WX`, and runs the turning mocks twice:
defaults, then five readings / 17% approach / 31% rotation / 19 encoder counts /
reversed left encoder polarity / 1500 ms timeout.
For GCC, from this directory:

```sh
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I. turn.c tests/test_turn.c -o test_turn
./test_turn
```

Both MSVC builds passed. Tests cover the six-sensor layout; left/right motor
signs; ignoring unrelated sensor readings during approach; encoder baselines,
distance thresholds, wraparound in both directions, reverse travel, and a stalled
wheel; requiring both front sensors to leave the original line; rejecting brief
background/black glitches and brief distance-threshold crossings; requiring both
sensors to reacquire; start/reset locking; IDLE hardware ownership; latched stopped
results; and exact total timeout in all three moving states, including a transition
on the deadline. **These are simulated sensor/motor/encoder tests, not physical robot tests
or a full PSoC firmware build.** The incomplete hardware adapter is not included
in those tests.
