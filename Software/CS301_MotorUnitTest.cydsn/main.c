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
#include <motorControl.h>
//* ========================================
#include "defines.h"
#include "vars.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================

int main()
{
    

// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    MotorInit();
    MotorLeft_setDirection(Forward);
    MotorRight_setDirection(Forward);
    MotorLeft_setRPM(30);
    MotorRight_setRPM(30);
    
    bool getSpeed_Uart;
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);

    //usbPutString(displaystring);
    
    for(;;)
    {   
        /* Place your application code here. */
        handle_usb();
        
        if (flag_KB_string == 1)
        {
            int targetSpeed;
            int duty;
            if(sscanf(line, "setSpeedL %d", &targetSpeed) == 1){
                MotorLeft_setRPM(targetSpeed);
                char setSpeedString[132];
                sprintf(setSpeedString, "setSpeedL to %d%%", targetSpeed);
                usbPutString("Speed Set to L");
            }
            if(sscanf(line, "setSpeedR %d", &targetSpeed) == 1){
                MotorRight_setRPM(targetSpeed);
                char setSpeedString[32];
                sprintf(setSpeedString, "setSpeedR to %d%%", targetSpeed);
                usbPutString("Speed set to R");
            }
            if(strcmp(line,"getSpeed") == 0){
                getSpeed_Uart = 1;
                usbPutString("getSpeed\r\n");
            }
            if(strcmp(line,"stopGetSpeed") == 0){
                getSpeed_Uart = 0;
                usbPutString("stopgetSpeed\r\n");
            }
            if(strcmp(line,"Forward") == 0){
                MotorLeft_setDirection(Forward);
                MotorRight_setDirection(Forward);
                usbPutString("fowarding");
            }
            if(strcmp(line,"Backward") == 0){
                MotorLeft_setDirection(Backward);
                MotorRight_setDirection(Backward);
                usbPutString("backwarding");
            }
            if(sscanf(line, "pl %d", &duty) == 1){
                PWM_1_WriteCompare(duty);
                usbPutString("duty cycle changed");
            }
            if(sscanf(line, "pr %d", &duty) == 1){
                PWM_2_WriteCompare(duty);
                usbPutString("duty cycle changed");
            }
            flag_KB_string = 0;
        } 
        if(Motor_isr_flag == 1){
            Motor_isr_flag = 0;
            Motor_captureRPM();
            //Motor_maintainSpeed();
            if(getSpeed_Uart){
                char speedString_L[32];
                char speedString_R[32];
                int motorSpeed_L =(int) -(MotorLeft_getRPM() *100);
                int motorSpeed_R =(int) (MotorRight_getRPM() *100);
                sprintf(speedString_L, "Left: %d.%d rpm\r\n",motorSpeed_L/100, abs(motorSpeed_L%100));
                sprintf(speedString_R, "Right: %d.%d rpm\r\n",motorSpeed_R/100, abs(motorSpeed_R%100));

                usbPutString(speedString_L);
                usbPutString(speedString_R);
            }
            
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