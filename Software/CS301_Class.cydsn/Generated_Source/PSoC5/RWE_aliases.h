/*******************************************************************************
* File Name: RWE.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_RWE_ALIASES_H) /* Pins RWE_ALIASES_H */
#define CY_PINS_RWE_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define RWE_0			(RWE__0__PC)
#define RWE_0_INTR	((uint16)((uint16)0x0001u << RWE__0__SHIFT))

#define RWE_INTR_ALL	 ((uint16)(RWE_0_INTR))

#endif /* End Pins RWE_ALIASES_H */


/* [] END OF FILE */
