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

//* ========================================
#include "defines.h"
#include "vars.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================

extern volatile int motorSpeed_tick;

const float Vblack_LON;
const float Vwhite_LON;

/* SysTick callback slot 0 provides a real 1 ms clock for ADC-stall detection. */
static volatile uint32 lineFollowMs;
static uint32 lastLineFrameMs;
static void LineFollowClock(void)
{
    ++lineFollowMs;
}

static void UpdateLineFollowing(const SensorFrame *frame, uint32 now)
{
    if (frame->fresh) {
        lastLineFrameMs = now;
        straightFromFrame(35, frame); /* Keep the current base speed. */
    } else if ((uint32)(now - lastLineFrameMs) >=
               LINE_FOLLOW_SENSOR_TIMEOUT_MS) {
        straightFromFrame(0, NULL); /* Sensor stream has stopped updating. */
    }
}

int main()
{
    

// --------------------------------    
// ----- INITIALIZATIONS ----------
    sensor_init();
    MotorInit();
    MotorDisable(); /* Wait for a valid frame before enabling movement. */
    //MotorLeft_setRPM(30);
    //MotorRight_setRPM(30);
    
    CYGlobalIntEnable;
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);

    CySysTickInit(); /* Existing CyLib uses a nominal 1 ms tick. */
    CySysTickSetCallback(0U, LineFollowClock);
    CySysTickEnable();

    //usbPutString(displaystring);
    
    for(;;)
    {   
        SensorFrame f =  sensors_GetFrame();
        if(f.fresh){
            LED_1_Write(f.state[Q1] == SENSOR_WHITE);
            LED_2_Write(f.state[Q2] == SENSOR_WHITE);
            LED_3_Write(f.state[Q3] == SENSOR_WHITE);
            LED_4_Write(f.state[Q4] == SENSOR_WHITE);
            LED_5_Write(f.state[Q5] == SENSOR_WHITE);
            LED_6_Write(f.state[Q6] == SENSOR_WHITE);
        }
        /* Reuse the LED snapshot: reading sensors_GetFrame again would clear
         * or consume freshness independently. Do not call this during a turn
         * when turn integration is added; that controller must own the motors.
         */
        UpdateLineFollowing(&f, lineFollowMs);
        handle_usb();
        
            flag_KB_string = 0;
        if(Motor_isr_flag ==1){
            Motor_isr_flag = 0;
            Motor_captureRPM();
            Motor_maintainSpeed();
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
    /* Do not let a disconnected USB host block the sensor timeout. */
    if (USBUART_CDCIsReady() == 0) return;
    s[63]='\0';
    s[62]='!';
    USBUART_PutData((uint8*)s,strlen(s));
#endif
}
//* ========================================
void usbPutChar(char c)
{
#ifdef USE_USB     
    if (USBUART_CDCIsReady() == 0) return;
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
                       usbPutChar('!');
                    }
                    else
                        entry[usbBufCount++] = c;  
                }  
            }
        }
    }    
}


/* [] END OF FILE */
