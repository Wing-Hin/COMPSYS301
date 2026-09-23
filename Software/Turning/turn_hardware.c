/* PSoC adapter for the standalone Task 1 main. Existing component names only.
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
    /* Adapter using names already in motorControl.c.
     * Confirm PWM mode, zero-duty stop behavior and motor polarity on hardware.
     * Both PWMs must already be started. The standalone main controller owns
     * motor duty during following AND turning: do not also run
     * Motor_maintainSpeed(), USB PWM commands, or motor-writing ISRs.
     */
    uint16_t leftMagnitude;
    uint16_t rightMagnitude;

    if (left > 100) left = 100;
    if (left < -100) left = -100;
    if (right > 100) right = 100;
    if (right < -100) right = -100;

    leftMagnitude = (uint16_t)(left < 0 ? -left : left);
    rightMagnitude = (uint16_t)(right < 0 ? -right : right);

    /* Remove drive before changing direction. Driver-specific reversal timing
     * and braking must be handled by your hardware implementation if needed.
     */
    PWM_1_WriteCompare(0);
    PWM_2_WriteCompare(0);
    if (left == 0 && right == 0) {
        MotorDisable();
        return;
    }

    /* MotorEnable writes direction pins too, so set directions AFTER enabling. */
    MotorEnable();
    MotorLeft_setDirection(left >= 0 ? Forward : Backward);
    MotorRight_setDirection(right >= 0 ? Forward : Backward);

    /* Scale nonnegative magnitudes to the existing PWM periods. NEVER feed
     * a negative command to WriteCompare. These are existing component APIs.
     */
    PWM_1_WriteCompare((uint8)((uint32_t)leftMagnitude *
                             PWM_1_ReadPeriod() / 100U));
    PWM_2_WriteCompare((uint8)((uint32_t)rightMagnitude *
                             PWM_2_ReadPeriod() / 100U));
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
