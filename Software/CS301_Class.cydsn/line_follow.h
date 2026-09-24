/* ========#include <project.h>===============================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H
#include "sensor.h"

/* Starting values in speed-percentage points; tune on the robot. */
#define LINE_FOLLOW_FRONT_GAIN 2
#define LINE_FOLLOW_REAR_GAIN 1
#define LINE_FOLLOW_SENSOR_TIMEOUT_MS 50UL

/* Caller supplies one current snapshot and enforces freshness/turn ownership.
 * NULL or zero speed stops. Fresh valid readings can subsequently resume.
 */
void straightFromFrame(uint8 speed, const SensorFrame *frame);
void straight(uint8 speed);
#endif
