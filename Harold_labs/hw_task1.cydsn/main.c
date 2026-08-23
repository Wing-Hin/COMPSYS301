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

#define NSAMPLES 500
volatile uint16 adcBuf[NSAMPLES];
volatile uint8  dacBuf[NSAMPLES];
volatile uint16 idx;
volatile uint8  bufFull; // use uint8 because PSoC has no bool type



CY_ISR_PROTO(EOC_Handler);        
CY_ISR(EOC_Handler){
    uint16 raw = ADC_SAR_1_GetResult16();
    uint8  d   = (uint8)((raw / 2) >> 4);
    VDAC8_1_SetValue(d);                   
    if (!bufFull) {
        adcBuf[idx] = raw;
        dacBuf[idx] = d;
        if (++idx >= NSAMPLES) { idx = 0; bufFull = 1; }
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    isr_1_StartEx(EOC_Handler);
    ADC_SAR_1_Start();               
    VDAC8_1_Start();
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    Timer_1_Start(); // trigger the entire chain.
    

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    while (USBUART_1_GetConfiguration() == 0) {}
    USBUART_1_CDC_Init();
    
    for(;;)
    { 
        
        if(bufFull){
            char line[32];
            for(uint16 i = 0; i < NSAMPLES ; i++){
                
                sprintf(line, "%u,%u,%u\r\n", i, adcBuf[i], dacBuf[i]);
                while (USBUART_1_CDCIsReady() == 0) {}  // CDCIsReady returns 0 while the endpoint is busy.

                USBUART_1_PutString(line);
            }    
            bufFull = 0;
        }
    }
}

/* [] END OF FILE */
