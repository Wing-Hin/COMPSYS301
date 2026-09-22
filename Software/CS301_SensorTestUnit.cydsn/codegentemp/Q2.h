/*******************************************************************************
* File Name: Q2.h  
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

#if !defined(CY_PINS_Q2_H) /* Pins Q2_H */
#define CY_PINS_Q2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Q2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Q2__PORT == 15 && ((Q2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Q2_Write(uint8 value);
void    Q2_SetDriveMode(uint8 mode);
uint8   Q2_ReadDataReg(void);
uint8   Q2_Read(void);
void    Q2_SetInterruptMode(uint16 position, uint16 mode);
uint8   Q2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Q2_SetDriveMode() function.
     *  @{
     */
        #define Q2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Q2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Q2_DM_RES_UP          PIN_DM_RES_UP
        #define Q2_DM_RES_DWN         PIN_DM_RES_DWN
        #define Q2_DM_OD_LO           PIN_DM_OD_LO
        #define Q2_DM_OD_HI           PIN_DM_OD_HI
        #define Q2_DM_STRONG          PIN_DM_STRONG
        #define Q2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Q2_MASK               Q2__MASK
#define Q2_SHIFT              Q2__SHIFT
#define Q2_WIDTH              1u

/* Interrupt constants */
#if defined(Q2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Q2_SetInterruptMode() function.
     *  @{
     */
        #define Q2_INTR_NONE      (uint16)(0x0000u)
        #define Q2_INTR_RISING    (uint16)(0x0001u)
        #define Q2_INTR_FALLING   (uint16)(0x0002u)
        #define Q2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Q2_INTR_MASK      (0x01u) 
#endif /* (Q2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Q2_PS                     (* (reg8 *) Q2__PS)
/* Data Register */
#define Q2_DR                     (* (reg8 *) Q2__DR)
/* Port Number */
#define Q2_PRT_NUM                (* (reg8 *) Q2__PRT) 
/* Connect to Analog Globals */                                                  
#define Q2_AG                     (* (reg8 *) Q2__AG)                       
/* Analog MUX bux enable */
#define Q2_AMUX                   (* (reg8 *) Q2__AMUX) 
/* Bidirectional Enable */                                                        
#define Q2_BIE                    (* (reg8 *) Q2__BIE)
/* Bit-mask for Aliased Register Access */
#define Q2_BIT_MASK               (* (reg8 *) Q2__BIT_MASK)
/* Bypass Enable */
#define Q2_BYP                    (* (reg8 *) Q2__BYP)
/* Port wide control signals */                                                   
#define Q2_CTL                    (* (reg8 *) Q2__CTL)
/* Drive Modes */
#define Q2_DM0                    (* (reg8 *) Q2__DM0) 
#define Q2_DM1                    (* (reg8 *) Q2__DM1)
#define Q2_DM2                    (* (reg8 *) Q2__DM2) 
/* Input Buffer Disable Override */
#define Q2_INP_DIS                (* (reg8 *) Q2__INP_DIS)
/* LCD Common or Segment Drive */
#define Q2_LCD_COM_SEG            (* (reg8 *) Q2__LCD_COM_SEG)
/* Enable Segment LCD */
#define Q2_LCD_EN                 (* (reg8 *) Q2__LCD_EN)
/* Slew Rate Control */
#define Q2_SLW                    (* (reg8 *) Q2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Q2_PRTDSI__CAPS_SEL       (* (reg8 *) Q2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Q2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Q2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Q2_PRTDSI__OE_SEL0        (* (reg8 *) Q2__PRTDSI__OE_SEL0) 
#define Q2_PRTDSI__OE_SEL1        (* (reg8 *) Q2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Q2_PRTDSI__OUT_SEL0       (* (reg8 *) Q2__PRTDSI__OUT_SEL0) 
#define Q2_PRTDSI__OUT_SEL1       (* (reg8 *) Q2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Q2_PRTDSI__SYNC_OUT       (* (reg8 *) Q2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Q2__SIO_CFG)
    #define Q2_SIO_HYST_EN        (* (reg8 *) Q2__SIO_HYST_EN)
    #define Q2_SIO_REG_HIFREQ     (* (reg8 *) Q2__SIO_REG_HIFREQ)
    #define Q2_SIO_CFG            (* (reg8 *) Q2__SIO_CFG)
    #define Q2_SIO_DIFF           (* (reg8 *) Q2__SIO_DIFF)
#endif /* (Q2__SIO_CFG) */

/* Interrupt Registers */
#if defined(Q2__INTSTAT)
    #define Q2_INTSTAT            (* (reg8 *) Q2__INTSTAT)
    #define Q2_SNAP               (* (reg8 *) Q2__SNAP)
    
	#define Q2_0_INTTYPE_REG 		(* (reg8 *) Q2__0__INTTYPE)
#endif /* (Q2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Q2_H */


/* [] END OF FILE */
