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
#include <stdlib.h>

#define BatteryVoltage 7.7
#define Forward true
#define Backward false
#define Max_safeSpeed 3.0f
#define WheelRadius 0.0325
#define Kp 0.5
#define Ki 0.0
#define Kd 0.0
#define Ts 0.1

volatile bool Motor_isr_flag;
volatile bool Motor_enabled;

volatile bool overflowL_flag;
volatile bool overflowR_flag;

void MotorInit();

void MotorEnable();
void MotorDisable();

void MotorLeft_setRPM(int speed);
float MotorLeft_getRPM();
void MotorLeft_setDirection(bool direction);

void MotorRight_setRPM(int speed);
float MotorRight_getRPM();
void MotorRight_setDirection(bool direction);

void Motor_maintainSpeed();

float Motor_getDistanceTravelled();
void Motor_resetDistanceCount();

void Motor_captureRPM();
/* [] END OF FILE */
