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

volatile bool Motor_isr_flag = 0;
//Variables initialise
static volatile float motorLeft_RPM;
static volatile float motorRight_RPM;
static volatile float motorLeft_targetRPM;
static volatile float motorRight_targetRPM;

static volatile int motorLeft_PWM;
static volatile int motorRight_PWM;

static volatile float previousError_L;
static volatile float previousError_R;

volatile int previousCount_L = 0;
volatile int previousCount_R = 0;
volatile int count_travelled = 0;
//


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


//Left Motor control
//speed control, input 0-99 indicating percentage
void MotorLeft_setRPM(int speed){
    motorLeft_targetRPM = speed /100 * Max_safeSpeed;
}
//getRPM
float MotorLeft_getRPM(){
    return motorLeft_RPM;
}
//Set direction
void MotorLeft_setDirection(bool direction){
    if(direction){
        LWV_Write(1);
    }
    else{
        LWV_Write(0);
    }
}
//Left Motor control


//Right Motor control
// control, input 0-99 indicating percentage
void MotorRight_setRPM(int speed){
    motorRight_targetRPM = speed /100 * Max_safeSpeed;
}
//getRPM
float MotorRight_getRPM(){
    return motorRight_RPM;
}
//set Direction
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
float errorL;
float previousErrorL;
float integralL;
float derivatvieL;

float errorR;
float previousErrorR;
float integralR;
float derivatvieR;
void Motor_maintainSpeed(){
    //Left
    previousError_L = errorL;
    errorL = motorLeft_targetRPM - motorLeft_RPM;
    integralL += errorL * Ts;
    derivatvieL = (errorL - previousError_L)/Ts;
    float correctionL = Kp * errorL + 
                        Ki * integralL +
                        Kd * derivatvieL;
    motorLeft_PWM = motorLeft_PWM + correctionL/0.04;
    //Right
    previousError_R = errorR;
    errorR = motorRight_targetRPM - motorRight_RPM;
    integralR += errorR * Ts;
    derivatvieR = (errorR - previousError_R)/Ts;
    float correctionR = Kp * errorR + 
                        Ki * integralR +
                        Kd * derivatvieR;
    motorRight_PWM = motorRight_PWM + correctionR/0.04;

}
//PID control


//Get distance travelled in meters
float Motor_getDistanceTravelled(){
    return count_travelled/(57*4)*2*3.1415926*WheelRadius;
}
//



//Speed(RPM) captures and calculations
void Motor_captureRPM(){
    motorLeft_RPM = (QuadDec_M1_GetCounter()-previousCount_L)/(57*4*0.1);
    motorRight_RPM = (QuadDec_M2_GetCounter()-previousCount_R)/(57*4*0.1);
    previousCount_L = QuadDec_M1_GetCounter();
    previousCount_R = QuadDec_M2_GetCounter();
    
    count_travelled += (QuadDec_M1_GetCounter()+QuadDec_M2_GetCounter())/2;
    
}
//
/* [] END OF FILE */
