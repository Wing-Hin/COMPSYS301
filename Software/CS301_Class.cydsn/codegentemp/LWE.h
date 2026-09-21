/*******************************************************************************
* File Name: LWE.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LWE_H) /* Pins LWE_H */
#define CY_PINS_LWE_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LWE_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LWE__PORT == 15 && ((LWE__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    LWE_Write(uint8 value);
void    LWE_SetDriveMode(uint8 mode);
uint8   LWE_ReadDataReg(void);
uint8   LWE_Read(void);
void    LWE_SetInterruptMode(uint16 position, uint16 mode);
uint8   LWE_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the LWE_SetDriveMode() function.
     *  @{
     */
        #define LWE_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define LWE_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define LWE_DM_RES_UP          PIN_DM_RES_UP
        #define LWE_DM_RES_DWN         PIN_DM_RES_DWN
        #define LWE_DM_OD_LO           PIN_DM_OD_LO
        #define LWE_DM_OD_HI           PIN_DM_OD_HI
        #define LWE_DM_STRONG          PIN_DM_STRONG
        #define LWE_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define LWE_MASK               LWE__MASK
#define LWE_SHIFT              LWE__SHIFT
#define LWE_WIDTH              1u

/* Interrupt constants */
#if defined(LWE__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LWE_SetInterruptMode() function.
     *  @{
     */
        #define LWE_INTR_NONE      (uint16)(0x0000u)
        #define LWE_INTR_RISING    (uint16)(0x0001u)
        #define LWE_INTR_FALLING   (uint16)(0x0002u)
        #define LWE_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define LWE_INTR_MASK      (0x01u) 
#endif /* (LWE__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define LWE_PS                     (* (reg8 *) LWE__PS)
/* Data Register */
#define LWE_DR                     (* (reg8 *) LWE__DR)
/* Port Number */
#define LWE_PRT_NUM                (* (reg8 *) LWE__PRT) 
/* Connect to Analog Globals */                                                  
#define LWE_AG                     (* (reg8 *) LWE__AG)                       
/* Analog MUX bux enable */
#define LWE_AMUX                   (* (reg8 *) LWE__AMUX) 
/* Bidirectional Enable */                                                        
#define LWE_BIE                    (* (reg8 *) LWE__BIE)
/* Bit-mask for Aliased Register Access */
#define LWE_BIT_MASK               (* (reg8 *) LWE__BIT_MASK)
/* Bypass Enable */
#define LWE_BYP                    (* (reg8 *) LWE__BYP)
/* Port wide control signals */                                                   
#define LWE_CTL                    (* (reg8 *) LWE__CTL)
/* Drive Modes */
#define LWE_DM0                    (* (reg8 *) LWE__DM0) 
#define LWE_DM1                    (* (reg8 *) LWE__DM1)
#define LWE_DM2                    (* (reg8 *) LWE__DM2) 
/* Input Buffer Disable Override */
#define LWE_INP_DIS                (* (reg8 *) LWE__INP_DIS)
/* LCD Common or Segment Drive */
#define LWE_LCD_COM_SEG            (* (reg8 *) LWE__LCD_COM_SEG)
/* Enable Segment LCD */
#define LWE_LCD_EN                 (* (reg8 *) LWE__LCD_EN)
/* Slew Rate Control */
#define LWE_SLW                    (* (reg8 *) LWE__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LWE_PRTDSI__CAPS_SEL       (* (reg8 *) LWE__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define LWE_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LWE__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define LWE_PRTDSI__OE_SEL0        (* (reg8 *) LWE__PRTDSI__OE_SEL0) 
#define LWE_PRTDSI__OE_SEL1        (* (reg8 *) LWE__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define LWE_PRTDSI__OUT_SEL0       (* (reg8 *) LWE__PRTDSI__OUT_SEL0) 
#define LWE_PRTDSI__OUT_SEL1       (* (reg8 *) LWE__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define LWE_PRTDSI__SYNC_OUT       (* (reg8 *) LWE__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(LWE__SIO_CFG)
    #define LWE_SIO_HYST_EN        (* (reg8 *) LWE__SIO_HYST_EN)
    #define LWE_SIO_REG_HIFREQ     (* (reg8 *) LWE__SIO_REG_HIFREQ)
    #define LWE_SIO_CFG            (* (reg8 *) LWE__SIO_CFG)
    #define LWE_SIO_DIFF           (* (reg8 *) LWE__SIO_DIFF)
#endif /* (LWE__SIO_CFG) */

/* Interrupt Registers */
#if defined(LWE__INTSTAT)
    #define LWE_INTSTAT            (* (reg8 *) LWE__INTSTAT)
    #define LWE_SNAP               (* (reg8 *) LWE__SNAP)
    
	#define LWE_0_INTTYPE_REG 		(* (reg8 *) LWE__0__INTTYPE)
#endif /* (LWE__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LWE_H */


/* [] END OF FILE */
