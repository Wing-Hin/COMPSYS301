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
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================

extern volatile int motorSpeed_tick;

const float Vblack_LON;
const float Vwhite_LON;

int main()
{
    

// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    PWM_1_Start();
    PWM_1_WritePeriod(99);
    PWM_1_WriteCompare(50);
    QuadDec_M1_Start();
    Timer_TS_Start();
    isr_TS_StartEx(isr_TS_Interrupt);
    ADC_Start();

    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);

    //usbPutString(displaystring);
    
    for(;;)
    {   
        /* Place your application code here. */
        if(ADC_GetResult16(0) <= 600){
            LED_1_Write(0);
        }
        else if(ADC_GetResult16(0) > 600){
            LED_1_Write(1);
        }
        if(ADC_GetResult16(1) <= 600){
            LED_2_Write(0);
        }
        else if(ADC_GetResult16(1) > 600){
            LED_2_Write(1);
        }
        handle_usb();
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT);
        
        if (flag_KB_string == 1)
        {
            int duty;
            int channel;
            if(sscanf(line, "p %d", &duty) == 1){
                PWM_1_WriteCompare(duty);
                usbPutString("duty cycle changed");
            }
            if(strcmp(line,"getSpeed") == 0){
                char speedString[16];
                char tickString[16];
                int motorSpeed =(int)(motorSpeed_tick*100/(0.5*4*57));
                sprintf(tickString, "%d ticks\r\n", motorSpeed_tick);
                sprintf(speedString, "%d.%d rpm\r\n",motorSpeed/100, motorSpeed%100);
                usbPutString(tickString);
                usbPutString(speedString);
            }
            if(sscanf(line, "ADC %d", &channel) == 1){
                char ADCnum_str[16];
                char ADCcount_str[16];
                int ADC_value = (int)ADC_GetResult16(channel);
                sprintf(ADCnum_str, "ADC%d reading:\r\n", channel);
                sprintf(ADCcount_str, "Count:%d\r\n", ADC_value);
                usbPutString(ADCnum_str);
                usbPutString(ADCcount_str);
                
            }
            flag_KB_string = 0;
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