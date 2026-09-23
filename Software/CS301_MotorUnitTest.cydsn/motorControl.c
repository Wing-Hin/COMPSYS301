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
volatile bool Motor_enabled = 0;

volatile bool overflowL_flag = 0;
volatile bool overflowR_flag = 0;

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

void MotorInit(){
    PWM_1_Start();
    PWM_1_WritePeriod(99);
    PWM_2_Start();
    PWM_2_WritePeriod(99);
    
    QuadDec_M1_Start();
    QuadDec_M1_SetInterruptMask(
    QuadDec_M1_COUNTER_OVERFLOW |
    QuadDec_M1_COUNTER_UNDERFLOW);
    isr_DEC1_StartEx(isr_DEC1_Interrupt);
    QuadDec_M2_Start();
    QuadDec_M2_SetInterruptMask(
    QuadDec_M2_COUNTER_OVERFLOW |
    QuadDec_M2_COUNTER_UNDERFLOW);
    isr_DEC2_StartEx(isr_DEC2_Interrupt);
    
    Timer_Motor_Start();
    isr_TM_StartEx(isr_TM_Interrupt);
    MotorEnable();
    MotorLeft_setDirection(Forward);
    MotorRight_setDirection(Forward);
}

//Enable Motor
void MotorEnable(){
    LWE_Write(1);
    RWE_Write(1);
    LWV_Write(1);
    RWV_Write(1);
    Motor_enabled = 1;
}
//Disable Motor
void MotorDisable(){
    LWE_Write(0);
    RWE_Write(0);
    Motor_enabled = 0;
}


//Left Motor control
//speed control, input 0-99 indicating percentage
void MotorLeft_setRPM(int speed){
    motorLeft_targetRPM = ((float)speed /100) * Max_safeSpeed;
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
    motorRight_targetRPM = ((float)speed /100) * Max_safeSpeed;
}
//getRPM
float MotorRight_getRPM(){
    return motorRight_RPM;
}
//set Direction
void MotorRight_setDirection(bool direction){
    if(direction){
        RWV_Write(0);
    }
    else{
        RWV_Write(1);
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
    if(Motor_enabled){
        //Left
        previousError_L = errorL;
        if(motorLeft_RPM < 0){
            errorL = motorLeft_targetRPM + motorLeft_RPM;
        }
        else{
            errorL = motorLeft_targetRPM - motorLeft_RPM;
        }
        integralL += errorL * Ts;
        derivatvieL = (errorL - previousError_L)/Ts;
        float correctionL = Kp * errorL + 
                            Ki * integralL +
                            Kd * derivatvieL;
        motorLeft_PWM = motorLeft_PWM - correctionL/0.04;
        if(motorLeft_PWM <0){
            motorLeft_PWM = 20;
        }
        else if(motorLeft_PWM > 99){
            motorLeft_PWM = 99;
        }
        PWM_1_WriteCompare(motorLeft_PWM);
        //Right
        previousError_R = errorR;
        if(motorRight_RPM < 0){
            errorR = motorRight_targetRPM + motorRight_RPM;
        }
        else{
            errorR = motorRight_targetRPM - motorRight_RPM;
        }
        integralR += errorR * Ts;
        derivatvieR = (errorR - previousError_R)/Ts;
        float correctionR = Kp * errorR + 
                            Ki * integralR +
                            Kd * derivatvieR;
        motorRight_PWM = motorRight_PWM - correctionR/0.04;
        if(motorRight_PWM <0){
            motorRight_PWM = 20;
        }
        else if(motorRight_PWM > 99){
            motorRight_PWM = 99;
        }
        PWM_2_WriteCompare(motorRight_PWM);
    }
}
//PID control


//Get distance travelled in meters
float Motor_getDistanceTravelled(){
    return count_travelled/(57*4)*2*3.1415926*WheelRadius;
}
//

void Motor_resetDistanceCount(){
    count_travelled = 0;
}

//Speed(RPM) captures and 
//left forward negative
//right backward negative
void Motor_captureRPM(){
    int currentCount_L = abs(QuadDec_M1_GetCounter());
    int currentCount_R = abs(QuadDec_M2_GetCounter());
    if(overflowL_flag){
        motorLeft_RPM = (currentCount_L-previousCount_L-32767)/(57*4*0.1);
    }else{
        motorLeft_RPM = (currentCount_L-previousCount_L)/(57*4*0.1);
    }
    if(overflowR_flag){
        motorRight_RPM = (currentCount_R-previousCount_R-32767)/(57*4*0.1);
    }else{
        motorRight_RPM = (currentCount_R-previousCount_R)/(57*4*0.1);
    }
    previousCount_L = currentCount_L;
    previousCount_R = currentCount_R;
    
    count_travelled += ((currentCount_L - previousCount_L)+
                       (currentCount_R - previousCount_R))/2;
    if(overflowL_flag || overflowR_flag){
        count_travelled += 32767/2;
        overflowL_flag = 0;
        overflowR_flag = 0;
    }
    
}
//
/* [] END OF FILE */
