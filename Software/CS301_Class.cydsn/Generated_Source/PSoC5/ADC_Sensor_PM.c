/*******************************************************************************
* File Name: ADC_Sensor_PM.c
* Version 2.10
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ADC_Sensor.h"
#include "ADC_Sensor_SAR.h"
#if(ADC_Sensor_CLOCK_SOURCE == ADC_Sensor_CLOCK_INTERNAL)
    #include "ADC_Sensor_IntClock.h"
#endif   /* ADC_Sensor_CLOCK_SOURCE == ADC_Sensor_CLOCK_INTERNAL */


/*******************************************************************************
* Function Name: ADC_Sensor_Sleep
********************************************************************************
*
* Summary:
*  Stops the ADC operation and saves the configuration registers and component
*  enable state. Should be called just prior to entering sleep
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ADC_Sensor_Sleep(void)
{
    ADC_Sensor_SAR_Stop();
    ADC_Sensor_SAR_Sleep();
    ADC_Sensor_Disable();

    #if(ADC_Sensor_CLOCK_SOURCE == ADC_Sensor_CLOCK_INTERNAL)
        ADC_Sensor_IntClock_Stop();
    #endif   /* ADC_Sensor_CLOCK_SOURCE == ADC_Sensor_CLOCK_INTERNAL */
}


/*******************************************************************************
* Function Name: ADC_Sensor_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component enable state and configuration registers. This should
*  be called just after awaking from sleep mode
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ADC_Sensor_Wakeup(void)
{
    ADC_Sensor_SAR_Wakeup();
    ADC_Sensor_SAR_Enable();

    #if(ADC_Sensor_CLOCK_SOURCE == ADC_Sensor_CLOCK_INTERNAL)
        ADC_Sensor_IntClock_Start();
    #endif   /* ADC_Sensor_CLOCK_SOURCE == ADC_Sensor_CLOCK_INTERNAL */

    /* The block is ready to use 10 us after the SAR enable signal is set high. */
    CyDelayUs(10u);
    
    ADC_Sensor_Enable();

    #if(ADC_Sensor_SAMPLE_MODE == ADC_Sensor_SAMPLE_MODE_FREE_RUNNING)
        ADC_Sensor_SAR_StartConvert();
    #endif /* (ADC_Sensor_SAMPLE_MODE == ADC_Sensor_SAMPLE_MODE_FREE_RUNNING) */

    (void) CY_GET_REG8(ADC_Sensor_STATUS_PTR);
}


/*******************************************************************************
* Function Name: ADC_Sensor_SaveConfig
********************************************************************************
*
* Summary:
*  Save the current configuration of ADC non-retention registers
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ADC_Sensor_SaveConfig(void)
{

}


/*******************************************************************************
* Function Name: ADC_Sensor_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the configuration of ADC non-retention registers
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ADC_Sensor_RestoreConfig(void)
{

}


/* [] END OF FILE */
