/* ========================================
 * Fully working code: 
 * PWM      : 
 * Encoder  : 
 * ADC      :
 * USB      : port displays speed and position.
 * CMD: "PW xx"
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ========================================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>
#include "line_follow.h"
#include "sensor.h"
#include "motorControl.h"

#include "robot_state.h"
#include "turn.h"
#include "turn_hardware.h"

//* ========================================
#include "defines.h"
#include "vars.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================

extern volatile int motorSpeed_tick;

SensorFrame f;
RobotState currentState;
RobotState previousState;

int main()
{
    

// --------------------------------    
// ----- INITIALIZATIONS ----------
    sensor_init();
    MotorInit();
    isr_TS_StartEx(isr_TS_Interrupt);
    //MotorLeft_setRPM(30);
    //MotorRight_setRPM(30);
    
    CYGlobalIntEnable;
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);

    //usbPutString(displaystring);
    
    for(;;)
    {   
        f =  sensors_GetFrame();
        
        if(f.fresh){
            LED_1_Write(getSingleSensorState(Q1) == SENSOR_WHITE);
            LED_2_Write(getSingleSensorState(Q2) == SENSOR_WHITE);
            LED_3_Write(f.state[Q3] == SENSOR_WHITE);
            LED_4_Write(f.state[Q4] == SENSOR_WHITE);
            LED_5_Write(f.state[Q5] == SENSOR_WHITE);
            LED_6_Write(f.state[Q6] == SENSOR_WHITE);
        }
        
        previousState = currentState;
        currentState = RobotDecideState(f.state);
        if(previousState != currentState && currentState == ROBOT_STATE_LEFT_BRANCH){
            TurnStart(TURN_LEFT);
        }
        else if(previousState != currentState && currentState == ROBOT_STATE_LEFT_BRANCH){
            TurnStart(TURN_RIGHT);
        }
        else if (currentState == ROBOT_STATE_FOLLOW_LINE && TurnGetState() == TURN_IDLE){
            straight(20);
        }

        handle_usb();
        
            flag_KB_string = 0;
        if(Motor_isr_flag ==1){
            Motor_isr_flag = 0;
            Motor_captureRPM();
            Motor_maintainSpeed();
            TurnUpdate();
        }
        
        if(Turn_isr_count >=7){
            Turn_isr_count=0;
            char string[32];
            //sprintf(string, "leftcount:%d \r\n right count:%d",leftTravelCounts, rightTravelCounts);
            //usbPutString(string);
            
            switch(TurnGetState()){
                case TURN_IDLE:
                    usbPutString("idle");
                    break;
                case TURN_APPROACH:
                    usbPutString("approach");
                    break;
                case TURN_LEAVE_LINE:
                    usbPutString("leave line");
                    break;
                case TURN_FIND_LINE:
                    usbPutString("find line");
                    break;
                case TURN_DONE:
                    usbPutString("done");
                    break;
                case TURN_FAULT:
                    usbPutString("fault");
                    break;
            }
            
            /*
            switch(currentState){
            case ROBOT_STATE_SENSOR_FAULT:
                usbPutString("fault");
                break;
            case ROBOT_STATE_LINE_LOST:
                usbPutString("line lost");
                break;
            case ROBOT_STATE_FOLLOW_LINE:
                usbPutString("follow");
                break;
            case ROBOT_STATE_LEFT_BRANCH:
                usbPutString("left");
                break;
            case ROBOT_STATE_RIGHT_BRANCH:
                usbPutString("right");
                break;
            case ROBOT_STATE_JUNCTION:
                usbPutString("junction");
                break;
            }
            */
        }
        
    }        
}   
//* ========================================
//* ========================================
void usbPutString(char *s)
{
// !! Assumes that *s is a string with allocated space >=64 chars     
//  Since USB implementation retricts data packets to 64 chars, this function truncates the
//  length to 62 char (63rd char is a '!')

#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    s[63]='\0';
    s[62]='!';
    USBUART_PutData((uint8*)s,strlen(s));
#endif
}
//* ========================================
void usbPutChar(char c)
{
#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    USBUART_PutChar(c);
#endif    
}
//* ========================================
void handle_usb()
{
    // handles input at terminal, echos it back to the terminal
    // turn echo OFF, key emulation: only CR
    // entered string is made available in 'line' and 'flag_KB_string' is set
    
    static uint8 usbStarted = FALSE;
    static uint16 usbBufCount = 0;
    uint8 c; 
    

    if (!usbStarted)
    {
        if (USBUART_GetConfiguration())
        {
            USBUART_CDC_Init();
            usbStarted = TRUE;
        }
    }
    else
    {
        if (USBUART_DataIsReady() != 0)
        {  
            c = USBUART_GetChar();

            if ((c == 13) || (c == 10))
            {
//                if (usbBufCount > 0)
                {
                    entry[usbBufCount]= '\0';
                    strcpy(line,entry);
                    usbBufCount = 0;
                    flag_KB_string = 1;
                }
            }
            else 
            {
                if (((c == CHAR_BACKSP) || (c == CHAR_DEL) ) && (usbBufCount > 0) )
                    usbBufCount--;
                else
                {
                    if (usbBufCount > (BUF_SIZE-2) ) // one less else strtok triggers a crash
                    {
                       USBUART_PutChar('!');        
                    }
                    else
                        entry[usbBufCount++] = c;  
                }  
            }
        }
    }    
}


/* [] END OF FILE */