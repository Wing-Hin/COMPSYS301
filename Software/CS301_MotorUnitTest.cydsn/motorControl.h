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
#define Forward true
#define Backward false
#define Max_safeSpeed 3.0f
#define Kp 1f
#define Ki 0.2f
#define Kd 0f
#define Ts 0.1f

void MotorEnable();
void MotorDisable();

void MotorLeft_setSpeed();
void MotorLeft_setDirection(bool direction);

void MotorRight_setSpeed();
void MotorRight_setDirection(bool direction);

void Motor_maintainSpeed();
/* [] END OF FILE */
