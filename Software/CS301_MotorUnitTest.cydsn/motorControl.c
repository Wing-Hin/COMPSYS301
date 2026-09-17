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
#include <motorControl.h>
extern volatile float motorLeft_speed;
extern volatile float motorRight_speed;
static float motorLeft_targetSpeed;
static float motorRight_targetSpeed;
static float previousError_L;
static float previousError_R;
//Enable Motor
void MotorEnable(){
    LWE_Write(1);
    RWE_Write(1);
    LWV_Write(1);
    RWV_Write(1);
}
//Disable Motor
void MotorDisable(){
    LWE_Write(0);
    RWE_Write(0);
}

//Left Motor speed control, input 0-99 indicating percentage
void MotorLeft_setSpeed(int speed){
    motorLeft_targetSpeed = speed /100 * Max_safeSpeed;
}
void MotorLeft_setDirection(bool direction){
    if(direction){
        LWV_Write(1);
    }
    else{
        LWV_Write(0);
    }
}
//Left Motor control

//Right Motor speed control, input 0-99 indicating percentage
void MotorRight_setSpeed(int speed){
    motorRight_targetSpeed = speed /100 * Max_safeSpeed;
}

void MotorRight_setDirection(bool direction){
    if(direction){
        RWV_Write(1);
    }
    else{
        RWV_Write(0);
    }
}
//Right motor control

//PID control
void Motor_maintainSpeed(){
    //Left
    //Right

}
/* [] END OF FILE */
