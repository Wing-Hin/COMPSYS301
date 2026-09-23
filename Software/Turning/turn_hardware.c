/* PSoC adapter for the project Task 1 controller. Existing component names only.
 * Verify sensor channel order, motor polarity and encoder signs on hardware.
 */
#include "turn.h"
#include "turn_hardware.h"
#include "motorControl.h"

#if N_SENSORS != 6
#error "This adapter expects the supplied six-sensor board"
#endif

SensorFrame controlSensorFrame;

uint8_t ReadSensor(SensorId sensor)
{
    static const uint8 channelForSensor[SENSOR_COUNT] = {Q3, Q6, Q5, Q4, Q1, Q2};
    /* main validates all channels and freshness before calling the controller.
     * Reading this cached frame avoids pair skew and repeated freshness reads.
     */
    return controlSensorFrame.state[channelForSensor[sensor]] == SENSOR_BLACK ? 1U : 0U;
}

void SetMotorSpeed(int16_t left, int16_t right)
{
    /* motorControl.c accepts a nonnegative 0..100 speed percentage for each
     * wheel. Direction and each wheel's enable are controlled separately.
     * The Timer_Motor service in main must continue to call Motor_captureRPM()
     * and Motor_maintainSpeed(); that service applies these targets to PWM.
     */
    static int8_t previousLeftSign;
    static int8_t previousRightSign;
    int8_t leftSign;
    int8_t rightSign;
    int leftMagnitude;
    int rightMagnitude;

    if (left > 100) left = 100;
    if (left < -100) left = -100;
    if (right > 100) right = 100;
    if (right < -100) right = -100;

    leftSign = left > 0 ? 1 : (left < 0 ? -1 : 0);
    rightSign = right > 0 ? 1 : (right < 0 ? -1 : 0);
    leftMagnitude = left < 0 ? -(int)left : (int)left;
    rightMagnitude = right < 0 ? -(int)right : (int)right;

    /* Disable a wheel before reversing its direction pin. There is no blocking
     * delay here; add one inside the hardware driver if the H-bridge requires a
     * dead time. Repeated commands in the same direction do not pulse the
     * enable pins off on every 5 ms controller update.
     */
    if (leftSign != 0 && previousLeftSign != 0 &&
        leftSign != previousLeftSign) {
        MotorLeft_stop();
    }
    if (rightSign != 0 && previousRightSign != 0 &&
        rightSign != previousRightSign) {
        MotorRight_stop();
    }

    /* Never pass signed values to the target-speed functions. A zero target is
     * installed before disabling so a later enable cannot intentionally resume
     * an old requested speed.
     */
    MotorLeft_setRPM(leftMagnitude);
    MotorRight_setRPM(rightMagnitude);

    if (left == 0 && right == 0) {
        MotorDisable();
        previousLeftSign = 0;
        previousRightSign = 0;
        return;
    }

    if (leftSign != 0 && leftSign != previousLeftSign) {
        MotorLeft_setDirection(leftSign > 0 ? Forward : Backward);
    }
    if (rightSign != 0 && rightSign != previousRightSign) {
        MotorRight_setDirection(rightSign > 0 ? Forward : Backward);
    }

    if (!Motor_enabled) MotorEnable();

    if (leftSign == 0) MotorLeft_stop();
    else MotorLeft_start();
    if (rightSign == 0) MotorRight_stop();
    else MotorRight_start();

    previousLeftSign = leftSign;
    previousRightSign = rightSign;
}

void ReadWheelEncoderCounts(int16_t *left, int16_t *right)
{
    /* Existing 16-bit, x4 quadrature components used by motorControl.c/tacho.c.
     * Start QuadDec_M1 and QuadDec_M2 once in your hardware initialization.
     * TurnStart saves a baseline; no hardware counter is reset here.
     */
    *left = QuadDec_M1_GetCounter();
    *right = QuadDec_M2_GetCounter();
}
