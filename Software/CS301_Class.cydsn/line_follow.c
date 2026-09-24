#include "line_follow.h"
#include "motorControl.h"
#include <stddef.h>

static void StopFollowing(void)
{
    /* Clear previous targets so a later enable cannot reuse an old request. */
    MotorLeft_setRPM(0);
    MotorRight_setRPM(0);
    MotorDisable();
}

static int ClampSpeed(int speed)
{
    if (speed < 0) return 0;
    if (speed > 99) return 99;
    return speed;
}

void straightFromFrame(uint8 speed, const SensorFrame *frame)
{
    static const uint8 channels[4] = {Q6, Q5, Q1, Q2};
    unsigned int i;
    int frontLeft, frontRight, rearLeft, rearRight;
    int frontVisible, rearVisible;
    int frontError, rearError, correction;
    int baseSpeed, leftSpeed, rightSpeed;

    if (frame == NULL || speed == 0) {
        StopFollowing();
        return;
    }
    /* Q3/Q4 are branch sensors, not steering inputs. Reject invalid values
     * on any of the four steering sensors before treating them as booleans.
     */
    for (i = 0; i < 4; ++i) {
        uint8 reading = frame->state[channels[i]];
        if (reading != SENSOR_BLACK && reading != SENSOR_WHITE) {
            StopFollowing();
            return;
        }
    }

    /* Physical layout: front Q6(left)/Q5(right), rear Q1(left)/Q2(right). */
    frontLeft = frame->state[Q6] == SENSOR_BLACK;
    frontRight = frame->state[Q5] == SENSOR_BLACK;
    rearLeft = frame->state[Q1] == SENSOR_BLACK;
    rearRight = frame->state[Q2] == SENSOR_BLACK;
    frontVisible = frontLeft || frontRight;
    rearVisible = rearLeft || rearRight;
    if (!frontVisible && !rearVisible) {
        StopFollowing(); /* All four white: no line to steer towards. */
        return;
    }

    /* Error = right-black minus left-black. +1 steers right, -1 left.
     * A missing pair contributes zero but is NOT considered centred.
     */
    frontError = frontVisible ? frontRight - frontLeft : 0;
    rearError = rearVisible ? rearRight - rearLeft : 0;
    correction = LINE_FOLLOW_FRONT_GAIN * frontError +
                 LINE_FOLLOW_REAR_GAIN * rearError;

    /* Keep the weighted steering direction, but limit the strength to avoid
     * large left/right command jumps. At base 10, correction gives 11/9 or 9/11.
     */
    if (correction > 1) correction = 1;
    if (correction < -1) correction = -1;

    baseSpeed = ClampSpeed(speed);
    if (!frontVisible || !rearVisible) {
        baseSpeed /= 2; /* Only one pair sees the line: reduce base speed. */
    }
    if (baseSpeed == 0) {
        StopFollowing();
        return;
    }
    leftSpeed = ClampSpeed(baseSpeed + correction);
    rightSpeed = ClampSpeed(baseSpeed - correction);

    /* Forward steering only. When integrating turning, caller must not run
     * this function during a turn; hand control back with the motors stopped.
     */
    MotorLeft_setRPM(leftSpeed);
    MotorRight_setRPM(rightSpeed);
    MotorLeft_setDirection(Forward);
    MotorRight_setDirection(Forward);
    if (!Motor_enabled) MotorEnable();
    if (leftSpeed == 0) MotorLeft_stop();
    else MotorLeft_start();
    if (rightSpeed == 0) MotorRight_stop();
    else MotorRight_start();
}

void straight(uint8 speed)
{
    /* Legacy wrapper for callers that have not already consumed a frame.
     * Main uses straightFromFrame instead. Freshness timeouts belong to the
     * caller because an absent new frame is normal between ADC windows.
     */
    SensorFrame frame = sensors_GetFrame();
    if (speed == 0 || frame.fresh) straightFromFrame(speed, &frame);
}
