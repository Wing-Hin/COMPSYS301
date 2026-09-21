/*******************************************************************************
* File Name: RWV.h  
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

#if !defined(CY_PINS_RWV_H) /* Pins RWV_H */
#define CY_PINS_RWV_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "RWV_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 RWV__PORT == 15 && ((RWV__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    RWV_Write(uint8 value);
void    RWV_SetDriveMode(uint8 mode);
uint8   RWV_ReadDataReg(void);
uint8   RWV_Read(void);
void    RWV_SetInterruptMode(uint16 position, uint16 mode);
uint8   RWV_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the RWV_SetDriveMode() function.
     *  @{
     */
        #define RWV_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define RWV_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define RWV_DM_RES_UP          PIN_DM_RES_UP
        #define RWV_DM_RES_DWN         PIN_DM_RES_DWN
        #define RWV_DM_OD_LO           PIN_DM_OD_LO
        #define RWV_DM_OD_HI           PIN_DM_OD_HI
        #define RWV_DM_STRONG          PIN_DM_STRONG
        #define RWV_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define RWV_MASK               RWV__MASK
#define RWV_SHIFT              RWV__SHIFT
#define RWV_WIDTH              1u

/* Interrupt constants */
#if defined(RWV__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in RWV_SetInterruptMode() function.
     *  @{
     */
        #define RWV_INTR_NONE      (uint16)(0x0000u)
        #define RWV_INTR_RISING    (uint16)(0x0001u)
        #define RWV_INTR_FALLING   (uint16)(0x0002u)
        #define RWV_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define RWV_INTR_MASK      (0x01u) 
#endif /* (RWV__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define RWV_PS                     (* (reg8 *) RWV__PS)
/* Data Register */
#define RWV_DR                     (* (reg8 *) RWV__DR)
/* Port Number */
#define RWV_PRT_NUM                (* (reg8 *) RWV__PRT) 
/* Connect to Analog Globals */                                                  
#define RWV_AG                     (* (reg8 *) RWV__AG)                       
/* Analog MUX bux enable */
#define RWV_AMUX                   (* (reg8 *) RWV__AMUX) 
/* Bidirectional Enable */                                                        
#define RWV_BIE                    (* (reg8 *) RWV__BIE)
/* Bit-mask for Aliased Register Access */
#define RWV_BIT_MASK               (* (reg8 *) RWV__BIT_MASK)
/* Bypass Enable */
#define RWV_BYP                    (* (reg8 *) RWV__BYP)
/* Port wide control signals */                                                   
#define RWV_CTL                    (* (reg8 *) RWV__CTL)
/* Drive Modes */
#define RWV_DM0                    (* (reg8 *) RWV__DM0) 
#define RWV_DM1                    (* (reg8 *) RWV__DM1)
#define RWV_DM2                    (* (reg8 *) RWV__DM2) 
/* Input Buffer Disable Override */
#define RWV_INP_DIS                (* (reg8 *) RWV__INP_DIS)
/* LCD Common or Segment Drive */
#define RWV_LCD_COM_SEG            (* (reg8 *) RWV__LCD_COM_SEG)
/* Enable Segment LCD */
#define RWV_LCD_EN                 (* (reg8 *) RWV__LCD_EN)
/* Slew Rate Control */
#define RWV_SLW                    (* (reg8 *) RWV__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define RWV_PRTDSI__CAPS_SEL       (* (reg8 *) RWV__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define RWV_PRTDSI__DBL_SYNC_IN    (* (reg8 *) RWV__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define RWV_PRTDSI__OE_SEL0        (* (reg8 *) RWV__PRTDSI__OE_SEL0) 
#define RWV_PRTDSI__OE_SEL1        (* (reg8 *) RWV__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define RWV_PRTDSI__OUT_SEL0       (* (reg8 *) RWV__PRTDSI__OUT_SEL0) 
#define RWV_PRTDSI__OUT_SEL1       (* (reg8 *) RWV__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define RWV_PRTDSI__SYNC_OUT       (* (reg8 *) RWV__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(RWV__SIO_CFG)
    #define RWV_SIO_HYST_EN        (* (reg8 *) RWV__SIO_HYST_EN)
    #define RWV_SIO_REG_HIFREQ     (* (reg8 *) RWV__SIO_REG_HIFREQ)
    #define RWV_SIO_CFG            (* (reg8 *) RWV__SIO_CFG)
    #define RWV_SIO_DIFF           (* (reg8 *) RWV__SIO_DIFF)
#endif /* (RWV__SIO_CFG) */

/* Interrupt Registers */
#if defined(RWV__INTSTAT)
    #define RWV_INTSTAT            (* (reg8 *) RWV__INTSTAT)
    #define RWV_SNAP               (* (reg8 *) RWV__SNAP)
    
	#define RWV_0_INTTYPE_REG 		(* (reg8 *) RWV__0__INTTYPE)
#endif /* (RWV__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_RWV_H */


/* [] END OF FILE */
