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
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <project.h>
#include <stdbool.h>

#define BatteryVoltage 7.7
#define Forward true
#define Backward false
#define Max_safeSpeed 3.0f
#define WheelRadius 0.0325
#define Kp 0.8
#define Ki 0.1
#define Kd 0.0
#define Ts 0.1

extern volatile bool Motor_isr_flag;
extern volatile bool Motor_enabled;

void MotorInit(void);

void MotorEnable(void);
void MotorDisable(void);

void MotorLeft_setRPM(int speed);
void MotorLeft_stop(void);
void MotorLeft_start(void);
float MotorLeft_getRPM(void);
void MotorLeft_setDirection(bool direction);

void MotorRight_setRPM(int speed);
void MotorRight_stop(void);
void MotorRight_start(void);
float MotorRight_getRPM(void);
void MotorRight_setDirection(bool direction);

void Motor_maintainSpeed(void);

float Motor_getDistanceTravelled(void);

void Motor_captureRPM(void);

#endif
/* [] END OF FILE */
