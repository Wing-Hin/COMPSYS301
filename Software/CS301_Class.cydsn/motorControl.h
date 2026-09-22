/* ========================================
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
#include <project.h>
#include <stdbool.h>

#define BatteryVoltage 7.7
#define Forward false
#define Backward true
#define Max_safeSpeed 3.0f
#define WheelRadius 0.0325
#define Kp 0.5
#define Ki 0.0
#define Kd 0.0
#define Ts 0.1

extern volatile bool Motor_isr_flag;

void MotorInit();

void MotorEnable();
void MotorDisable();

void MotorLeft_setRPM();
float MotorLeft_getRPM();
void MotorLeft_setDirection(bool direction);

void MotorRight_setRPM();
float MotorRight_getRPM();
void MotorRight_setDirection(bool direction);

void Motor_maintainSpeed();

float Motor_getDistanceTravelled();

void Motor_captureRPM();
/* [] END OF FILE */
