#ifndef TURN_HARDWARE_H
#define TURN_HARDWARE_H

#include "sensor.h"

/* Main refreshes this once per tick using the existing sensors_GetFrame().
 * ADC interrupts update the driver's own frame, not this copy. Every sensor
 * read in one controller update therefore sees the same six-channel sample.
 */
extern SensorFrame controlSensorFrame;

/* SetMotorSpeed updates the existing motorControl.c targets. Main must service
 * Motor_isr_flag by calling Motor_captureRPM() and Motor_maintainSpeed(), using
 * the cadence configured by Timer_Motor. Do not run another motion controller
 * (such as straight()) at the same time because it would replace the targets.
 */

#endif
