/*******************************************************************************
* File Name: Q4.h  
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

#if !defined(CY_PINS_Q4_H) /* Pins Q4_H */
#define CY_PINS_Q4_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Q4_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Q4__PORT == 15 && ((Q4__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Q4_Write(uint8 value);
void    Q4_SetDriveMode(uint8 mode);
uint8   Q4_ReadDataReg(void);
uint8   Q4_Read(void);
void    Q4_SetInterruptMode(uint16 position, uint16 mode);
uint8   Q4_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Q4_SetDriveMode() function.
     *  @{
     */
        #define Q4_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Q4_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Q4_DM_RES_UP          PIN_DM_RES_UP
        #define Q4_DM_RES_DWN         PIN_DM_RES_DWN
        #define Q4_DM_OD_LO           PIN_DM_OD_LO
        #define Q4_DM_OD_HI           PIN_DM_OD_HI
        #define Q4_DM_STRONG          PIN_DM_STRONG
        #define Q4_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Q4_MASK               Q4__MASK
#define Q4_SHIFT              Q4__SHIFT
#define Q4_WIDTH              1u

/* Interrupt constants */
#if defined(Q4__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Q4_SetInterruptMode() function.
     *  @{
     */
        #define Q4_INTR_NONE      (uint16)(0x0000u)
        #define Q4_INTR_RISING    (uint16)(0x0001u)
        #define Q4_INTR_FALLING   (uint16)(0x0002u)
        #define Q4_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Q4_INTR_MASK      (0x01u) 
#endif /* (Q4__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Q4_PS                     (* (reg8 *) Q4__PS)
/* Data Register */
#define Q4_DR                     (* (reg8 *) Q4__DR)
/* Port Number */
#define Q4_PRT_NUM                (* (reg8 *) Q4__PRT) 
/* Connect to Analog Globals */                                                  
#define Q4_AG                     (* (reg8 *) Q4__AG)                       
/* Analog MUX bux enable */
#define Q4_AMUX                   (* (reg8 *) Q4__AMUX) 
/* Bidirectional Enable */                                                        
#define Q4_BIE                    (* (reg8 *) Q4__BIE)
/* Bit-mask for Aliased Register Access */
#define Q4_BIT_MASK               (* (reg8 *) Q4__BIT_MASK)
/* Bypass Enable */
#define Q4_BYP                    (* (reg8 *) Q4__BYP)
/* Port wide control signals */                                                   
#define Q4_CTL                    (* (reg8 *) Q4__CTL)
/* Drive Modes */
#define Q4_DM0                    (* (reg8 *) Q4__DM0) 
#define Q4_DM1                    (* (reg8 *) Q4__DM1)
#define Q4_DM2                    (* (reg8 *) Q4__DM2) 
/* Input Buffer Disable Override */
#define Q4_INP_DIS                (* (reg8 *) Q4__INP_DIS)
/* LCD Common or Segment Drive */
#define Q4_LCD_COM_SEG            (* (reg8 *) Q4__LCD_COM_SEG)
/* Enable Segment LCD */
#define Q4_LCD_EN                 (* (reg8 *) Q4__LCD_EN)
/* Slew Rate Control */
#define Q4_SLW                    (* (reg8 *) Q4__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Q4_PRTDSI__CAPS_SEL       (* (reg8 *) Q4__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Q4_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Q4__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Q4_PRTDSI__OE_SEL0        (* (reg8 *) Q4__PRTDSI__OE_SEL0) 
#define Q4_PRTDSI__OE_SEL1        (* (reg8 *) Q4__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Q4_PRTDSI__OUT_SEL0       (* (reg8 *) Q4__PRTDSI__OUT_SEL0) 
#define Q4_PRTDSI__OUT_SEL1       (* (reg8 *) Q4__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Q4_PRTDSI__SYNC_OUT       (* (reg8 *) Q4__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Q4__SIO_CFG)
    #define Q4_SIO_HYST_EN        (* (reg8 *) Q4__SIO_HYST_EN)
    #define Q4_SIO_REG_HIFREQ     (* (reg8 *) Q4__SIO_REG_HIFREQ)
    #define Q4_SIO_CFG            (* (reg8 *) Q4__SIO_CFG)
    #define Q4_SIO_DIFF           (* (reg8 *) Q4__SIO_DIFF)
#endif /* (Q4__SIO_CFG) */

/* Interrupt Registers */
#if defined(Q4__INTSTAT)
    #define Q4_INTSTAT            (* (reg8 *) Q4__INTSTAT)
    #define Q4_SNAP               (* (reg8 *) Q4__SNAP)
    
	#define Q4_0_INTTYPE_REG 		(* (reg8 *) Q4__0__INTTYPE)
#endif /* (Q4__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Q4_H */


/* [] END OF FILE */
