# Task 1 controller: line following and automatic right-angle corners

This folder now contains a real `main.c`, `main_control.c/.h`, and
`turn_hardware.c/.h`, in addition to the existing turning state machine.
It is intended for the benchmark path with left/right corners and **no
intersections**. It includes basic line following; there are no route-planning
or line-following placeholder callbacks. Task 2 distance-D stopping is not included.

## Add to PSoC Creator

1. In `Software/CS301_Class.cydsn`, exclude the old ADC/USB test `main.c` from
   the build and add this folder's `main.c` instead. Compile exactly one main.
2. Add `main_control.c`, `turn.c`, and `turn_hardware.c`, plus their headers and
   `turn_config.h`. Keep the existing `sensor.c/.h`, `motorControl.c/.h`, and
   generated component sources in the project. Include both source directories.
3. Do not compile either `.example` file or the host `tests` folder into firmware.
   The old examples now point to the real implementation.
4. Build in PSoC Creator, program the robot, and verify mapping, motor polarity,
   encoder scaling and line geometry at low speed. No generated project files
   or existing driver/draft files were edited by this change.

**Startup behaviour:** the standalone main stops the motors, starts both encoders
and the existing sensor driver, and configures the existing CyLib SysTick API
for a 1 ms callback. Every fifth callback schedules one control update in main.
After the default **two-second delay**, it automatically starts when both outer
sensors are clear and all four middle sensors see black. Place the robot centred
on a straight segment before resetting the board. Reset for the next run.

Main takes one `sensors_GetFrame()` snapshot per tick. All ReadSensor calls use
that copy, so a controller-wide interrupt mask is unnecessary. Unknown/stale
sensor data after the startup delay stops and latches a fault. The main detects
missed control ticks rather than replaying them. No blocking ADC waits, USB
output, extra motor regulator, or manual PWM commands are in this main loop.
An entirely stalled main still needs a hardware watchdog for guaranteed stopping.

The public controller API is:

| Function | Purpose |
| --- | --- |
| `MainControlStart()` | Start once after initialization and valid sensor input |
| `MainControlUpdate5ms()` | Sole scheduled motion update; calls TurnUpdate internally |
| `MainControlGetState()` | STOPPED, FOLLOWING, TURNING, or FAULT |
| `MainControlGetFault()` | Reason for a stopped fault; inspect in debugger |
| `MainControlGetCompletedTurns()` | Count confirmed completed turns for this run |
| `MainControlHalt(reason)` | Stop and latch a sensor/scheduler/application fault |

The supplied main already makes these calls. Do not separately call TurnUpdate.
It never automatically recovers a fault. Reset the board after correcting one.
The API permits explicit restart after idle/terminal faults, but a hardware halt
while the turn state machine is moving requires board reset.

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

- `ReadSensor`: main reuses `sensors_GetFrame()` from `sensor.c`; the adapter reads
  the cached frame and
  converts its `SENSOR_BLACK == 0` convention to the turning module's black=1.
  Run `sensor_init()` once. The binary interface requires valid input: exclude
  `SENSOR_UNKNOWN` and stale frames before using it; neither means background.
  The existing `getSingleSensorState()` has an off-by-one bounds check, so the
  adapter uses the cached frame. The driver comments describe an 8 ms frame window;
  5 ms reads can repeat a frame. Tune filtering against the actual frame rate,
  or update acquisition to provide a fresh frame each tick. Repeated observations
  are not independent ADC samples.
- `SetMotorSpeed`: the adapter reuses `MotorEnable`, `MotorDisable`,
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

## Automatic corner detection and motor handoff

The controller reads all six sensors while following:

| Confirmed pattern | Action |
| --- | --- |
| FL=1, FR=0, BM_L=1, BM_R=1 | Request left turn |
| FL=0, FR=1, BM_L=1, BM_R=1 | Request right turn |
| FL=1 and FR=1 | Stop; sustained detection faults as ambiguous |
| No reliable front guidance and rear pair not both black | Stop; sustained loss faults |

A left/right candidate must persist for `CORNER_CONFIRM_READINGS` updates.
A change in direction or loss of the pattern resets its counter. While confirming,
the robot creeps forward. This motion precedes the TurnStart encoder baseline,
so calibrate `TURN_APPROACH_COUNTS` from the **confirmed** detection position.
Rear-pair agreement reduces false turns caused by lateral drift but cannot prove
that the observed shape is a right-angle corner. Check patterns on the lab track.
The front pair need not be black for corner detection: they can already have
passed the edge of the bend while the rear pair remains on the incoming line.

The basic follower drives equally when both front middle sensors are black.
With only the left black it slows the left wheel; with only the right black it
slows the right wheel. With both front sensors on background and both rear middle
sensors black it creeps straight; otherwise it stops and confirms line loss.
This is deliberately simple steering, not a tuned PID or speed regulator. It
adapts the slower-inner-wheel idea from the unfinished `Abdur Code/line_follow.c`
without modifying or compiling that draft.

After a confirmed corner, TurnStart drives the encoder-based approach. During
APPROACH/LEAVE_LINE/FIND_LINE the main controller returns after TurnUpdate, so
normal following cannot overwrite its motor commands. On DONE it counts the turn,
acknowledges it with TurnReset and resumes following on the next tick. A turn
FAULT latches a stopped controller fault. Outer detections are ignored after a
turn until both outers are clear AND all four middle sensors are black for several
normal-following updates. This prevents a second request at the same corner.
There must be enough centred straight travel between bends to rearm detection.

The controller owns motor duty even while TurnGetState() is IDLE: normal following
also uses SetMotorSpeed. Do not run Motor_maintainSpeed or manual motor writers
alongside this standalone controller. The turn module still follows
`IDLE -> APPROACH -> LEAVE_LINE -> FIND_LINE -> DONE`, with total timeout to FAULT.

## Geometry assumptions and limits

The intended branch is approximately perpendicular to the approach line. At
rotation start, the wheel midpoint must be close enough to the intersection
centre and the robot centred on the incoming line. Calibrate forward travel from
the actual front-corner trigger to that pivot position. Front-sensor
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
numbers there; the modules include it and validate their settings at compile time.
Keep `TURN_UPDATE_MS` at 5 to match the scheduler contract.

Controller defaults: follow duty 25%, slower inner-wheel duty 12%, corner
confirmation 3 readings, line-loss confirmation 6 readings, startup delay 2000 ms,
and stale-sensor timeout 30 ms. These settings are in the same config file.

Defaults: approach 20%, rotation 25%, forward travel 34 encoder counts
per wheel, three consecutive confirmations per transition, and 4000 ms for the
entire manoeuvre. **34 counts is an uncalibrated initial estimate**, about 30 mm
using the existing motor code's 57*4 counts per revolution and 65 mm wheel diameter:

```text
approach_counts = distance_mm * counts_per_wheel_revolution / (pi * wheel_diameter_mm)
```

Confirm the actual wheel counts per revolution, wheel diameter and front-sensor
to axle distance. The drawing's 29.97 mm row spacing is not proof of axle position.
Tune the threshold for the actual corner trigger and confirmation travel.
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

The runner builds with MSVC `/std:c11 /W4 /WX`. It compile-checks the PSoC
entry point and adapter against small host API declaration stubs, then runs the
turn-state tests and integrated controller tests with default and alternate
configurations. All four executable test runs passed.

The turn tests cover directions, encoder wrap/reverse/stall, debounce, original-line
rejection, ownership, result locking and timeout. The integrated tests cover
steering, brief/alternating corner signals, rejecting an uncentred rear pattern,
11 alternating simulated turns, same-corner suppression, rearming, return to
following, motor ownership and stopped ambiguous/line-loss/turn/hardware faults.

**This is simulated verification, not physical robot testing or a full generated
PSoC firmware build.** Startup scheduling and peripherals are compile-checked,
not dynamically emulated. Actual sensor patterns, axle offset, approach count,
encoder polarity, PWM behaviour and stopping drift still require lab calibration.
