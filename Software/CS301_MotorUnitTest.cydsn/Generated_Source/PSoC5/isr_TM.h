/*******************************************************************************
* File Name: isr_TM.h
* Version 1.71
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_isr_TM_H)
#define CY_ISR_isr_TM_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void isr_TM_Start(void);
void isr_TM_StartEx(cyisraddress address);
void isr_TM_Stop(void);

CY_ISR_PROTO(isr_TM_Interrupt);

void isr_TM_SetVector(cyisraddress address);
cyisraddress isr_TM_GetVector(void);

void isr_TM_SetPriority(uint8 priority);
uint8 isr_TM_GetPriority(void);

void isr_TM_Enable(void);
uint8 isr_TM_GetState(void);
void isr_TM_Disable(void);

void isr_TM_SetPending(void);
void isr_TM_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the isr_TM ISR. */
#define isr_TM_INTC_VECTOR            ((reg32 *) isr_TM__INTC_VECT)

/* Address of the isr_TM ISR priority. */
#define isr_TM_INTC_PRIOR             ((reg8 *) isr_TM__INTC_PRIOR_REG)

/* Priority of the isr_TM interrupt. */
#define isr_TM_INTC_PRIOR_NUMBER      isr_TM__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable isr_TM interrupt. */
#define isr_TM_INTC_SET_EN            ((reg32 *) isr_TM__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the isr_TM interrupt. */
#define isr_TM_INTC_CLR_EN            ((reg32 *) isr_TM__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the isr_TM interrupt state to pending. */
#define isr_TM_INTC_SET_PD            ((reg32 *) isr_TM__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the isr_TM interrupt. */
#define isr_TM_INTC_CLR_PD            ((reg32 *) isr_TM__INTC_CLR_PD_REG)


#endif /* CY_ISR_isr_TM_H */


/* [] END OF FILE */
