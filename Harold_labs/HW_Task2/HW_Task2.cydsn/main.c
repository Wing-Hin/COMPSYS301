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
#include "project.h"
#include <stdio.h>
#include <stdlib.h>


#define COUNTS_PER_SHAFT_REV  228.0f
#define TICK_SECONDS    1.0f

volatile int16  last_count = 0;
volatile int16  position   = 0;
volatile float  rpm        = 0;
volatile uint8  new_sample = 0; //flag


CY_ISR(Timer_ISR_Handler)
{
    /* Read (and clear) status FIRST - clearing at the end is too late on
       PSoC5 level interrupts and the ISR re-enters immediately. */
    uint8 status = Timer_1_ReadStatusRegister();

    /* Only measure on a genuine terminal-count event; ignore any
       capture/spurious events that re-fire this ISR faster than 1 Hz. */
    if (status & Timer_1_STATUS_TC)
    {
        int16 now   = QuadDec_1_GetCounter();
        int16 delta = now - last_count;
        last_count  = now;

        position = now;
        rpm = (delta * 60.0f) / (COUNTS_PER_SHAFT_REV * TICK_SECONDS);
        new_sample = 1;
    }
}

int main(void)
{
    
    char buffer[64];
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    
    PWM_1_Start();
    QuadDec_1_Start();
    isr_1_StartEx(Timer_ISR_Handler);
    Timer_1_Start();
    
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    while (USBUART_1_GetConfiguration() == 0) {}   /* wait for PC enumeration */
    USBUART_1_CDC_Init();
    
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    for(;;){
        if(USBUART_1_DataIsReady()){
            char c =  USBUART_1_GetChar();
            
            if (c == 's')
            {
                uint8 duty;
                USBUART_1_PutString("duty,rpm10\r\n");

                for (duty = 0; duty <= 100; duty += 5)
                {
                    PWM_1_WriteCompare(duty);

                    new_sample = 0;
                    while (!new_sample);  new_sample = 0;
                    while (!new_sample);  new_sample = 0;

                    sprintf(buffer, "%d,%d\r\n", duty, (int)(rpm * 10));
                    while (!USBUART_1_CDCIsReady());
                    USBUART_1_PutString(buffer);
                }

                PWM_1_WriteCompare(50);
                USBUART_1_PutString("done\r\n");
            }
            
            PWM_1_WriteCompare(50);
            USBUART_1_PutString("done \r\n");
        }
    }
}

/* [] END OF FILE */
