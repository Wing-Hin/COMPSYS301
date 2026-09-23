/* Standalone Task 1 entry point for CS301_Class.cydsn.
 * Add this main.c IN PLACE OF the old ADC/USB test main, not alongside it.
 * Link main_control.c, turn.c, turn_hardware.c, existing sensor.c and
 * motorControl.c, plus the PSoC generated sources. No route planner is needed.
 *
 * Starts automatically after CONTROL_START_DELAY_MS with valid centred input.
 * Reset the board to start a fresh run. Faults remain stopped until reset.
 * Observe MainControlGetFault()/MainControlGetCompletedTurns() in the debugger.
 * This is Task 1 only; there is no distance-D odometry-test mode here.
 */
#include <project.h>
#include "main_control.h"
#include "turn_hardware.h"

#if CONTROL_START_DELAY_MS < 0 || CONTROL_START_DELAY_MS > 60000UL || \
    (CONTROL_START_DELAY_MS % TURN_UPDATE_MS) != 0
#error "Startup delay must be 0..60000 ms, in multiples of 5 ms"
#endif
#if CONTROL_SENSOR_TIMEOUT_MS < TURN_UPDATE_MS || \
    CONTROL_SENSOR_TIMEOUT_MS > 60000UL || \
    (CONTROL_SENSOR_TIMEOUT_MS % TURN_UPDATE_MS) != 0
#error "Sensor timeout must be 5..60000 ms, in multiples of 5 ms"
#endif

static volatile uint8 pendingControlTicks;

/* The existing PSoC CyLib SysTick API defaults to a 1 ms interrupt. Slot 0
 * belongs to this standalone main. Do not also install another 5 ms updater.
 * The ISR only schedules work; all controller/motor functions run in main.
 */
static void ControlClock1ms(void)
{
    static uint8 milliseconds;
    if (++milliseconds >= TURN_UPDATE_MS) {
        milliseconds = 0;
        if (pendingControlTicks < 2U) ++pendingControlTicks;
    }
}

int main(void)
{
    uint32 startupTicks = 0;
    uint32 staleTicks = 0;
    bool started = false;

    PWM_1_Start();
    PWM_2_Start();
    SetMotorSpeed(0, 0);
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    sensor_init();

    CySysTickInit(); /* Configure 1 ms and initialize callbacks before setting ours. */
    (void)CySysTickSetCallback(0, ControlClock1ms);
    CySysTickEnable();
    CYGlobalIntEnable;

    for (;;) {
        uint8 due;
        uint8 interruptState;
        unsigned i;
        bool valid = true;

        interruptState = CyEnterCriticalSection();
        due = pendingControlTicks;
        pendingControlTicks = 0;
        CyExitCriticalSection(interruptState);
        if (due == 0U) continue;

        /* Never replay missed sensor samples or silently slow the turn timeout. */
        if (due > 1U) MainControlHalt(MAIN_FAULT_SCHEDULER);
        if (MainControlGetState() == MAIN_FAULT) {
            (void)MainControlUpdate5ms(); /* Keep commanding stopped. */
            continue;
        }

        controlSensorFrame = sensors_GetFrame();
        if (controlSensorFrame.fresh) staleTicks = 0;
        else ++staleTicks;

        for (i = 0; i < N_SENSORS; ++i) {
            if (controlSensorFrame.state[i] != SENSOR_BLACK &&
                controlSensorFrame.state[i] != SENSOR_WHITE) valid = false;
        }
        if (!started) {
            SetMotorSpeed(0, 0);
            if (startupTicks < CONTROL_START_DELAY_MS / TURN_UPDATE_MS) {
                ++startupTicks;
                continue;
            }
        }
        /* Unknown is NOT converted into background. No motion control runs
         * with invalid/stale data. The current sensor driver averages ~8 ms,
         * so a fresh frame is not required on every individual 5 ms tick.
         */
        if (!valid || staleTicks >= CONTROL_SENSOR_TIMEOUT_MS / TURN_UPDATE_MS) {
            MainControlHalt(MAIN_FAULT_SENSOR);
            continue;
        }
        if (!started) {
            /* Start on a straight centred part of the path, before the corner. */
            if (ReadSensor(SENSOR_FL) || ReadSensor(SENSOR_FR) ||
                !ReadSensor(SENSOR_FM_L) || !ReadSensor(SENSOR_FM_R) ||
                !ReadSensor(SENSOR_BM_L) || !ReadSensor(SENSOR_BM_R)) continue;
            started = MainControlStart();
            if (!started) MainControlHalt(MAIN_FAULT_REQUEST);
        }
        if (started) (void)MainControlUpdate5ms();

        /* No blocking ADC waits, USB output, manual PWM commands or separate
         * Motor_maintainSpeed here: the controller already owns motor duty.
         * A stalled main cannot service this timeout; system watchdog setup
         * remains a hardware-level responsibility.
         */
    }
}
