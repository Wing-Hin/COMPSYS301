# Turn-only controller

This folder implements a non-blocking left/right corner manoeuvre. It does not
perform normal line following, detect a corner, or choose a direction. The
application owns those responsibilities and calls `TurnStart(TURN_LEFT)` or
`TurnStart(TURN_RIGHT)` once it has decided to turn.

## Firmware files

- `turn.c/.h`: hardware-independent turn state machine.
- `turn_config.h`: speeds, encoder distance, confirmation count and timeout.
- `turn_hardware.c/.h`: adapter to the CS301 sensor, motor and encoder drivers.

Do not add the `.example` file or anything under `tests/` to the PSoC firmware
build. Keep `Software/CS301_Class.cydsn/main.c` as the only `main()` function.

## Integration

Add `turn.c` and `turn_hardware.c`, their headers, and `turn_config.h` to the
`CS301_Class` project. Keep the existing `sensor.c/.h` and
`motorControl.c/.h`. Add the Turning folder to the compiler include paths.

Main must:

1. Call `MotorInit()` and `sensor_init()` once.
2. Refresh `controlSensorFrame` from `sensors_GetFrame()` before each update.
3. Reject `SENSOR_UNKNOWN` or stale input before allowing motion.
4. Call `TurnStart()` once when another module requests a left or right turn.
5. Call `TurnUpdate()` exactly once every 5 ms while a turn is active.
6. Stop the normal line follower from replacing motor targets until the turn is
   complete.
7. Continue servicing the existing motor driver at its timer cadence:

```c
if (Motor_isr_flag == 1) {
    Motor_isr_flag = 0;
    Motor_captureRPM();
    Motor_maintainSpeed();
}
```

A typical handoff is:

```c
if (TurnGetState() == TURN_IDLE) {
    /* Normal controller may run here. Call TurnStart(direction) when needed. */
} else {
    TurnState result = TurnUpdate();
    if (result == TURN_DONE || result == TURN_FAULT) {
        /* Handle the result first, then acknowledge it. */
        TurnReset();
    }
}
```

Do not call `straight()` while `TurnGetState()` is `TURN_APPROACH`,
`TURN_LEAVE_LINE`, or `TURN_FIND_LINE`.

## Turn sequence

```text
IDLE -> APPROACH -> LEAVE_LINE -> FIND_LINE -> DONE
                                           \-> FAULT on timeout
```

- `APPROACH`: both wheels move forward until each has travelled
  `TURN_APPROACH_COUNTS` from the baseline saved by `TurnStart()`.
- `LEAVE_LINE`: the robot rotates until both front-middle sensors see
  background for `TURN_CONFIRM_READINGS` consecutive updates.
- `FIND_LINE`: rotation continues until both front-middle sensors see black for
  `TURN_CONFIRM_READINGS` consecutive updates.
- `DONE`: motors are stopped until the application handles the result and calls
  `TurnReset()`.
- `FAULT`: the total `TURN_TIMEOUT_MS` expired; motors remain stopped until the
  result is handled and reset.

The timeout covers the entire approach and rotation. `TurnReset()` is ignored
while the manoeuvre is still moving; it is not an emergency-stop API.

## Sensor and motor mapping

The internal sensor convention is black=1 and background=0. The adapter converts
the existing sensor driver's `SENSOR_BLACK == 0` values and maps:

| Logical position | Existing sensor |
| --- | --- |
| FL | Q3 |
| FM_L | Q6 |
| FM_R | Q5 |
| FR | Q4 |
| BM_L | Q1 |
| BM_R | Q2 |

Only FM_L and FM_R are used by the turn state machine. Main still refreshes the
complete cached frame so all reads in an update are coherent.

`SetMotorSpeed(left, right)` uses signed commands internally: positive means
forward, negative means reverse and zero means stop. The hardware adapter never
passes a negative target to `motorControl.c`; it uses the sign with
`MotorLeft_setDirection()`/`MotorRight_setDirection()` and passes the magnitude
to `MotorLeft_setRPM()`/`MotorRight_setRPM()`.

Encoder counters are read without resetting them. The state machine handles
signed 16-bit wraparound and requires both wheels to reach the approach distance.
Set each encoder sign in `turn_config.h` so forward travel accumulates positively.

## Configuration

Defaults in `turn_config.h` are starting values, not physical calibration:

| Setting | Default | Purpose |
| --- | ---: | --- |
| `TURN_APPROACH_SPEED` | 20 | Forward command before rotation |
| `TURN_ROTATE_SPEED` | 25 | Rotation command |
| `TURN_APPROACH_COUNTS` | 34 | Required forward encoder counts per wheel |
| `TURN_CONFIRM_READINGS` | 3 | Consecutive readings for each transition |
| `TURN_TIMEOUT_MS` | 4000 ms | Maximum total manoeuvre time |
| `TURN_UPDATE_MS` | 5 ms | Required update period |

Verify motor polarity, encoder polarity, approach distance, line geometry,
stopping drift and reversal behaviour on the physical robot at low speed.

## Tests

`tests/test_turn.c` covers left/right turns, encoder wraparound, reverse and
stalled movement, debounce, result locking and timeout. `tests/test_turn_hardware.c`
checks sensor mapping and translation to the real `motorControl.h` interface.

On Windows with Visual Studio C tools installed, run from the repository root:

```bat
Software\Turning\tests\run_tests.cmd
```

The runner executes the hardware-adapter test and the turn-state tests with
default and alternate configurations. These are host simulations, not a PSoC
firmware build or physical-track verification.
