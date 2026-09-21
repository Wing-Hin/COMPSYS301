/*******************************************************************************
* File Name: ADC_Sensor_PM.c
* Version 3.10
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ADC_Sensor.h"


/***************************************
* Local data allocation
***************************************/

static ADC_Sensor_BACKUP_STRUCT  ADC_Sensor_backup =
{
    ADC_Sensor_DISABLED
};


/*******************************************************************************
* Function Name: ADC_Sensor_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC_Sensor_SaveConfig(void)
{
    /* All configuration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC_Sensor_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC_Sensor_RestoreConfig(void)
{
    /* All congiguration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC_Sensor_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred routine to prepare the component for sleep.
*  The ADC_Sensor_Sleep() routine saves the current component state,
*  then it calls the ADC_Stop() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC_Sensor_backup - The structure field 'enableState' is modified
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void ADC_Sensor_Sleep(void)
{
    if((ADC_Sensor_PWRMGR_SAR_REG  & ADC_Sensor_ACT_PWR_SAR_EN) != 0u)
    {
        if((ADC_Sensor_SAR_CSR0_REG & ADC_Sensor_SAR_SOF_START_CONV) != 0u)
        {
            ADC_Sensor_backup.enableState = ADC_Sensor_ENABLED | ADC_Sensor_STARTED;
        }
        else
        {
            ADC_Sensor_backup.enableState = ADC_Sensor_ENABLED;
        }
        ADC_Sensor_Stop();
    }
    else
    {
        ADC_Sensor_backup.enableState = ADC_Sensor_DISABLED;
    }
}


/*******************************************************************************
* Function Name: ADC_Sensor_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred routine to restore the component to the state when
*  ADC_Sensor_Sleep() was called. If the component was enabled before the
*  ADC_Sensor_Sleep() function was called, the
*  ADC_Sensor_Wakeup() function also re-enables the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC_Sensor_backup - The structure field 'enableState' is used to
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void ADC_Sensor_Wakeup(void)
{
    if(ADC_Sensor_backup.enableState != ADC_Sensor_DISABLED)
    {
        ADC_Sensor_Enable();
        #if(ADC_Sensor_DEFAULT_CONV_MODE != ADC_Sensor__HARDWARE_TRIGGER)
            if((ADC_Sensor_backup.enableState & ADC_Sensor_STARTED) != 0u)
            {
                ADC_Sensor_StartConvert();
            }
        #endif /* End ADC_Sensor_DEFAULT_CONV_MODE != ADC_Sensor__HARDWARE_TRIGGER */
    }
}


/* [] END OF FILE */
