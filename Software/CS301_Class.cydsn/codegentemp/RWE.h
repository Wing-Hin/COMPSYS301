/*******************************************************************************
* File Name: RWE.h  
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

#if !defined(CY_PINS_RWE_H) /* Pins RWE_H */
#define CY_PINS_RWE_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "RWE_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 RWE__PORT == 15 && ((RWE__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    RWE_Write(uint8 value);
void    RWE_SetDriveMode(uint8 mode);
uint8   RWE_ReadDataReg(void);
uint8   RWE_Read(void);
void    RWE_SetInterruptMode(uint16 position, uint16 mode);
uint8   RWE_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the RWE_SetDriveMode() function.
     *  @{
     */
        #define RWE_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define RWE_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define RWE_DM_RES_UP          PIN_DM_RES_UP
        #define RWE_DM_RES_DWN         PIN_DM_RES_DWN
        #define RWE_DM_OD_LO           PIN_DM_OD_LO
        #define RWE_DM_OD_HI           PIN_DM_OD_HI
        #define RWE_DM_STRONG          PIN_DM_STRONG
        #define RWE_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define RWE_MASK               RWE__MASK
#define RWE_SHIFT              RWE__SHIFT
#define RWE_WIDTH              1u

/* Interrupt constants */
#if defined(RWE__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in RWE_SetInterruptMode() function.
     *  @{
     */
        #define RWE_INTR_NONE      (uint16)(0x0000u)
        #define RWE_INTR_RISING    (uint16)(0x0001u)
        #define RWE_INTR_FALLING   (uint16)(0x0002u)
        #define RWE_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define RWE_INTR_MASK      (0x01u) 
#endif /* (RWE__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define RWE_PS                     (* (reg8 *) RWE__PS)
/* Data Register */
#define RWE_DR                     (* (reg8 *) RWE__DR)
/* Port Number */
#define RWE_PRT_NUM                (* (reg8 *) RWE__PRT) 
/* Connect to Analog Globals */                                                  
#define RWE_AG                     (* (reg8 *) RWE__AG)                       
/* Analog MUX bux enable */
#define RWE_AMUX                   (* (reg8 *) RWE__AMUX) 
/* Bidirectional Enable */                                                        
#define RWE_BIE                    (* (reg8 *) RWE__BIE)
/* Bit-mask for Aliased Register Access */
#define RWE_BIT_MASK               (* (reg8 *) RWE__BIT_MASK)
/* Bypass Enable */
#define RWE_BYP                    (* (reg8 *) RWE__BYP)
/* Port wide control signals */                                                   
#define RWE_CTL                    (* (reg8 *) RWE__CTL)
/* Drive Modes */
#define RWE_DM0                    (* (reg8 *) RWE__DM0) 
#define RWE_DM1                    (* (reg8 *) RWE__DM1)
#define RWE_DM2                    (* (reg8 *) RWE__DM2) 
/* Input Buffer Disable Override */
#define RWE_INP_DIS                (* (reg8 *) RWE__INP_DIS)
/* LCD Common or Segment Drive */
#define RWE_LCD_COM_SEG            (* (reg8 *) RWE__LCD_COM_SEG)
/* Enable Segment LCD */
#define RWE_LCD_EN                 (* (reg8 *) RWE__LCD_EN)
/* Slew Rate Control */
#define RWE_SLW                    (* (reg8 *) RWE__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define RWE_PRTDSI__CAPS_SEL       (* (reg8 *) RWE__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define RWE_PRTDSI__DBL_SYNC_IN    (* (reg8 *) RWE__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define RWE_PRTDSI__OE_SEL0        (* (reg8 *) RWE__PRTDSI__OE_SEL0) 
#define RWE_PRTDSI__OE_SEL1        (* (reg8 *) RWE__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define RWE_PRTDSI__OUT_SEL0       (* (reg8 *) RWE__PRTDSI__OUT_SEL0) 
#define RWE_PRTDSI__OUT_SEL1       (* (reg8 *) RWE__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define RWE_PRTDSI__SYNC_OUT       (* (reg8 *) RWE__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(RWE__SIO_CFG)
    #define RWE_SIO_HYST_EN        (* (reg8 *) RWE__SIO_HYST_EN)
    #define RWE_SIO_REG_HIFREQ     (* (reg8 *) RWE__SIO_REG_HIFREQ)
    #define RWE_SIO_CFG            (* (reg8 *) RWE__SIO_CFG)
    #define RWE_SIO_DIFF           (* (reg8 *) RWE__SIO_DIFF)
#endif /* (RWE__SIO_CFG) */

/* Interrupt Registers */
#if defined(RWE__INTSTAT)
    #define RWE_INTSTAT            (* (reg8 *) RWE__INTSTAT)
    #define RWE_SNAP               (* (reg8 *) RWE__SNAP)
    
	#define RWE_0_INTTYPE_REG 		(* (reg8 *) RWE__0__INTTYPE)
#endif /* (RWE__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_RWE_H */


/* [] END OF FILE */
