/*******************************************************************************
* File Name: Timer_Motor_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_Motor.h"

static Timer_Motor_backupStruct Timer_Motor_backup;


/*******************************************************************************
* Function Name: Timer_Motor_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Motor_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Motor_SaveConfig(void) 
{
    #if (!Timer_Motor_UsingFixedFunction)
        Timer_Motor_backup.TimerUdb = Timer_Motor_ReadCounter();
        Timer_Motor_backup.InterruptMaskValue = Timer_Motor_STATUS_MASK;
        #if (Timer_Motor_UsingHWCaptureCounter)
            Timer_Motor_backup.TimerCaptureCounter = Timer_Motor_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Motor_UDB_CONTROL_REG_REMOVED)
            Timer_Motor_backup.TimerControlRegister = Timer_Motor_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Motor_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Motor_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Motor_RestoreConfig(void) 
{   
    #if (!Timer_Motor_UsingFixedFunction)

        Timer_Motor_WriteCounter(Timer_Motor_backup.TimerUdb);
        Timer_Motor_STATUS_MASK =Timer_Motor_backup.InterruptMaskValue;
        #if (Timer_Motor_UsingHWCaptureCounter)
            Timer_Motor_SetCaptureCount(Timer_Motor_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Motor_UDB_CONTROL_REG_REMOVED)
            Timer_Motor_WriteControlRegister(Timer_Motor_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Motor_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Motor_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Motor_Sleep(void) 
{
    #if(!Timer_Motor_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Motor_CTRL_ENABLE == (Timer_Motor_CONTROL & Timer_Motor_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Motor_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Motor_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Motor_Stop();
    Timer_Motor_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Motor_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Motor_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Motor_Wakeup(void) 
{
    Timer_Motor_RestoreConfig();
    #if(!Timer_Motor_UDB_CONTROL_REG_REMOVED)
        if(Timer_Motor_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Motor_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
