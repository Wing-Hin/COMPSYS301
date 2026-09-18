/*******************************************************************************
* File Name: Q3.h  
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

#if !defined(CY_PINS_Q3_H) /* Pins Q3_H */
#define CY_PINS_Q3_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Q3_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Q3__PORT == 15 && ((Q3__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Q3_Write(uint8 value);
void    Q3_SetDriveMode(uint8 mode);
uint8   Q3_ReadDataReg(void);
uint8   Q3_Read(void);
void    Q3_SetInterruptMode(uint16 position, uint16 mode);
uint8   Q3_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Q3_SetDriveMode() function.
     *  @{
     */
        #define Q3_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Q3_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Q3_DM_RES_UP          PIN_DM_RES_UP
        #define Q3_DM_RES_DWN         PIN_DM_RES_DWN
        #define Q3_DM_OD_LO           PIN_DM_OD_LO
        #define Q3_DM_OD_HI           PIN_DM_OD_HI
        #define Q3_DM_STRONG          PIN_DM_STRONG
        #define Q3_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Q3_MASK               Q3__MASK
#define Q3_SHIFT              Q3__SHIFT
#define Q3_WIDTH              1u

/* Interrupt constants */
#if defined(Q3__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Q3_SetInterruptMode() function.
     *  @{
     */
        #define Q3_INTR_NONE      (uint16)(0x0000u)
        #define Q3_INTR_RISING    (uint16)(0x0001u)
        #define Q3_INTR_FALLING   (uint16)(0x0002u)
        #define Q3_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Q3_INTR_MASK      (0x01u) 
#endif /* (Q3__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Q3_PS                     (* (reg8 *) Q3__PS)
/* Data Register */
#define Q3_DR                     (* (reg8 *) Q3__DR)
/* Port Number */
#define Q3_PRT_NUM                (* (reg8 *) Q3__PRT) 
/* Connect to Analog Globals */                                                  
#define Q3_AG                     (* (reg8 *) Q3__AG)                       
/* Analog MUX bux enable */
#define Q3_AMUX                   (* (reg8 *) Q3__AMUX) 
/* Bidirectional Enable */                                                        
#define Q3_BIE                    (* (reg8 *) Q3__BIE)
/* Bit-mask for Aliased Register Access */
#define Q3_BIT_MASK               (* (reg8 *) Q3__BIT_MASK)
/* Bypass Enable */
#define Q3_BYP                    (* (reg8 *) Q3__BYP)
/* Port wide control signals */                                                   
#define Q3_CTL                    (* (reg8 *) Q3__CTL)
/* Drive Modes */
#define Q3_DM0                    (* (reg8 *) Q3__DM0) 
#define Q3_DM1                    (* (reg8 *) Q3__DM1)
#define Q3_DM2                    (* (reg8 *) Q3__DM2) 
/* Input Buffer Disable Override */
#define Q3_INP_DIS                (* (reg8 *) Q3__INP_DIS)
/* LCD Common or Segment Drive */
#define Q3_LCD_COM_SEG            (* (reg8 *) Q3__LCD_COM_SEG)
/* Enable Segment LCD */
#define Q3_LCD_EN                 (* (reg8 *) Q3__LCD_EN)
/* Slew Rate Control */
#define Q3_SLW                    (* (reg8 *) Q3__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Q3_PRTDSI__CAPS_SEL       (* (reg8 *) Q3__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Q3_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Q3__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Q3_PRTDSI__OE_SEL0        (* (reg8 *) Q3__PRTDSI__OE_SEL0) 
#define Q3_PRTDSI__OE_SEL1        (* (reg8 *) Q3__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Q3_PRTDSI__OUT_SEL0       (* (reg8 *) Q3__PRTDSI__OUT_SEL0) 
#define Q3_PRTDSI__OUT_SEL1       (* (reg8 *) Q3__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Q3_PRTDSI__SYNC_OUT       (* (reg8 *) Q3__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Q3__SIO_CFG)
    #define Q3_SIO_HYST_EN        (* (reg8 *) Q3__SIO_HYST_EN)
    #define Q3_SIO_REG_HIFREQ     (* (reg8 *) Q3__SIO_REG_HIFREQ)
    #define Q3_SIO_CFG            (* (reg8 *) Q3__SIO_CFG)
    #define Q3_SIO_DIFF           (* (reg8 *) Q3__SIO_DIFF)
#endif /* (Q3__SIO_CFG) */

/* Interrupt Registers */
#if defined(Q3__INTSTAT)
    #define Q3_INTSTAT            (* (reg8 *) Q3__INTSTAT)
    #define Q3_SNAP               (* (reg8 *) Q3__SNAP)
    
	#define Q3_0_INTTYPE_REG 		(* (reg8 *) Q3__0__INTTYPE)
#endif /* (Q3__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Q3_H */


/* [] END OF FILE */
