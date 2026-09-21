/*******************************************************************************
* File Name: Q5.h  
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

#if !defined(CY_PINS_Q5_H) /* Pins Q5_H */
#define CY_PINS_Q5_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Q5_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Q5__PORT == 15 && ((Q5__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Q5_Write(uint8 value);
void    Q5_SetDriveMode(uint8 mode);
uint8   Q5_ReadDataReg(void);
uint8   Q5_Read(void);
void    Q5_SetInterruptMode(uint16 position, uint16 mode);
uint8   Q5_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Q5_SetDriveMode() function.
     *  @{
     */
        #define Q5_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Q5_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Q5_DM_RES_UP          PIN_DM_RES_UP
        #define Q5_DM_RES_DWN         PIN_DM_RES_DWN
        #define Q5_DM_OD_LO           PIN_DM_OD_LO
        #define Q5_DM_OD_HI           PIN_DM_OD_HI
        #define Q5_DM_STRONG          PIN_DM_STRONG
        #define Q5_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Q5_MASK               Q5__MASK
#define Q5_SHIFT              Q5__SHIFT
#define Q5_WIDTH              1u

/* Interrupt constants */
#if defined(Q5__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Q5_SetInterruptMode() function.
     *  @{
     */
        #define Q5_INTR_NONE      (uint16)(0x0000u)
        #define Q5_INTR_RISING    (uint16)(0x0001u)
        #define Q5_INTR_FALLING   (uint16)(0x0002u)
        #define Q5_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Q5_INTR_MASK      (0x01u) 
#endif /* (Q5__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Q5_PS                     (* (reg8 *) Q5__PS)
/* Data Register */
#define Q5_DR                     (* (reg8 *) Q5__DR)
/* Port Number */
#define Q5_PRT_NUM                (* (reg8 *) Q5__PRT) 
/* Connect to Analog Globals */                                                  
#define Q5_AG                     (* (reg8 *) Q5__AG)                       
/* Analog MUX bux enable */
#define Q5_AMUX                   (* (reg8 *) Q5__AMUX) 
/* Bidirectional Enable */                                                        
#define Q5_BIE                    (* (reg8 *) Q5__BIE)
/* Bit-mask for Aliased Register Access */
#define Q5_BIT_MASK               (* (reg8 *) Q5__BIT_MASK)
/* Bypass Enable */
#define Q5_BYP                    (* (reg8 *) Q5__BYP)
/* Port wide control signals */                                                   
#define Q5_CTL                    (* (reg8 *) Q5__CTL)
/* Drive Modes */
#define Q5_DM0                    (* (reg8 *) Q5__DM0) 
#define Q5_DM1                    (* (reg8 *) Q5__DM1)
#define Q5_DM2                    (* (reg8 *) Q5__DM2) 
/* Input Buffer Disable Override */
#define Q5_INP_DIS                (* (reg8 *) Q5__INP_DIS)
/* LCD Common or Segment Drive */
#define Q5_LCD_COM_SEG            (* (reg8 *) Q5__LCD_COM_SEG)
/* Enable Segment LCD */
#define Q5_LCD_EN                 (* (reg8 *) Q5__LCD_EN)
/* Slew Rate Control */
#define Q5_SLW                    (* (reg8 *) Q5__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Q5_PRTDSI__CAPS_SEL       (* (reg8 *) Q5__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Q5_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Q5__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Q5_PRTDSI__OE_SEL0        (* (reg8 *) Q5__PRTDSI__OE_SEL0) 
#define Q5_PRTDSI__OE_SEL1        (* (reg8 *) Q5__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Q5_PRTDSI__OUT_SEL0       (* (reg8 *) Q5__PRTDSI__OUT_SEL0) 
#define Q5_PRTDSI__OUT_SEL1       (* (reg8 *) Q5__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Q5_PRTDSI__SYNC_OUT       (* (reg8 *) Q5__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Q5__SIO_CFG)
    #define Q5_SIO_HYST_EN        (* (reg8 *) Q5__SIO_HYST_EN)
    #define Q5_SIO_REG_HIFREQ     (* (reg8 *) Q5__SIO_REG_HIFREQ)
    #define Q5_SIO_CFG            (* (reg8 *) Q5__SIO_CFG)
    #define Q5_SIO_DIFF           (* (reg8 *) Q5__SIO_DIFF)
#endif /* (Q5__SIO_CFG) */

/* Interrupt Registers */
#if defined(Q5__INTSTAT)
    #define Q5_INTSTAT            (* (reg8 *) Q5__INTSTAT)
    #define Q5_SNAP               (* (reg8 *) Q5__SNAP)
    
	#define Q5_0_INTTYPE_REG 		(* (reg8 *) Q5__0__INTTYPE)
#endif /* (Q5__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Q5_H */


/* [] END OF FILE */
