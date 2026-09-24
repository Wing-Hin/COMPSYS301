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
//* ========================================
#include "defines.h"
#include "vars.h"
#include "sensor.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================
volatile int count;

int main()
{

// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    sensor_init();
<<<<<<< Updated upstream
    //LED_1_Write(0);

=======
    isr_TS_StartEx(isr_TS_Interrupt);
    //LED_1_Write(0)
>>>>>>> Stashed changes

// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    for(;;){   
        SensorFrame f =  sensors_GetFrame();
        if(f.fresh){
            LED_1_Write(f.state[Q1] == SENSOR_WHITE);
            LED_2_Write(f.state[Q2] == SENSOR_WHITE);
            LED_3_Write(f.state[Q3] == SENSOR_WHITE);
            LED_4_Write(f.state[Q4] == SENSOR_WHITE);
            LED_5_Write(f.state[Q5] == SENSOR_WHITE);
            LED_6_Write(f.state[Q6] == SENSOR_WHITE);
        }
<<<<<<< Updated upstream
      
=======
        if(count >= 250){
            count= 0;
            char sensorString[256];
            sprintf(sensorString, "sensor1:%d\r\n"
            "sesnro2:%d\r\n"
            "sensor3:%d\r\n"
            "sensor4:%d\r\n"
            "sensor5:%d\r\n"
            "sensor6:%d\r\n",
            f.state[Q1],f.state[Q2],f.state[Q3],f.state[Q4],f.state[Q5],f.state[Q6]);
            usbPutString(sensorString);
        }
>>>>>>> Stashed changes
    }   
}
//* ========================================
void usbPutString(char *s)
{
// !! Assumes that *s is a string with allocted space >=64 chars     
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
