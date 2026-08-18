
/* ========================================
 * Laboratory Exercise 3
 *
 * Program timer to flash LED at 1Hz, use interrupts
 *
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ========================================
*/
#include <project.h>

CY_ISR(isr_handler){
    Timer_1_ReadStatusRegister();   /* acknowledge, drops the interrupt line */
    LED_Write(!LED_Read());          /* toggle */
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    Timer_1_Start();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    isr_TC_StartEx(isr_handler);

    for(;;)
    {
        /* Place your application code here. */
        
    }
}

/* [] END OF FILE */
