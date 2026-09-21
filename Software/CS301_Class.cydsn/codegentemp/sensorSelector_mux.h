/*******************************************************************************
* File Name: sensorSelector_mux.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUX_sensorSelector_mux_H)
#define CY_AMUX_sensorSelector_mux_H

#include "cyfitter.h"
#include "cyfitter_cfg.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))    
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif /* ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) */


/***************************************
*        Function Prototypes
***************************************/

void sensorSelector_mux_Start(void) ;
#define sensorSelector_mux_Init() sensorSelector_mux_Start()
void sensorSelector_mux_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void sensorSelector_mux_Stop(void); */
/* void sensorSelector_mux_Select(uint8 channel); */
/* void sensorSelector_mux_Connect(uint8 channel); */
/* void sensorSelector_mux_Disconnect(uint8 channel); */
/* void sensorSelector_mux_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define sensorSelector_mux_CHANNELS  6u
#define sensorSelector_mux_MUXTYPE   1
#define sensorSelector_mux_ATMOSTONE 1

/***************************************
*             API Constants
***************************************/

#define sensorSelector_mux_NULL_CHANNEL 0xFFu
#define sensorSelector_mux_MUX_SINGLE   1
#define sensorSelector_mux_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if sensorSelector_mux_MUXTYPE == sensorSelector_mux_MUX_SINGLE
# if !sensorSelector_mux_ATMOSTONE
#  define sensorSelector_mux_Connect(channel) sensorSelector_mux_Set(channel)
# endif
# define sensorSelector_mux_Disconnect(channel) sensorSelector_mux_Unset(channel)
#else
# if !sensorSelector_mux_ATMOSTONE
void sensorSelector_mux_Connect(uint8 channel) ;
# endif
void sensorSelector_mux_Disconnect(uint8 channel) ;
#endif

#if sensorSelector_mux_ATMOSTONE
# define sensorSelector_mux_Stop() sensorSelector_mux_DisconnectAll()
# define sensorSelector_mux_Select(channel) sensorSelector_mux_FastSelect(channel)
void sensorSelector_mux_DisconnectAll(void) ;
#else
# define sensorSelector_mux_Stop() sensorSelector_mux_Start()
void sensorSelector_mux_Select(uint8 channel) ;
# define sensorSelector_mux_DisconnectAll() sensorSelector_mux_Start()
#endif

#endif /* CY_AMUX_sensorSelector_mux_H */


/* [] END OF FILE */
