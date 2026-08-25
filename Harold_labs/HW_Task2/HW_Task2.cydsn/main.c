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
volatile uint8  new_sample = 0;


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
    
    
    PWM_1_WriteCompare(25);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;){
        if (new_sample){
            new_sample = 0;
            int rpm10 = (int)(rpm * 10);
            sprintf(buffer, "pos=%d  rpm=%s%d.%d\r\n", position,
        (rpm10 < 0) ? "-" : "", abs(rpm10) / 10, abs(rpm10) % 10);
            while (USBUART_1_CDCIsReady() == 0) {}
            USBUART_1_PutString(buffer);
        }
    }
}

/* [] END OF FILE */
