/*******************************************************************************
* File Name: sensorSelector_mux.c
* Version 1.80
*
*  Description:
*    This file contains all functions required for the analog multiplexer
*    AMux User Module.
*
*   Note:
*
*******************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "sensorSelector_mux.h"

static uint8 sensorSelector_mux_lastChannel = sensorSelector_mux_NULL_CHANNEL;


/*******************************************************************************
* Function Name: sensorSelector_mux_Start
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void sensorSelector_mux_Start(void) 
{
    uint8 chan;

    for(chan = 0u; chan < sensorSelector_mux_CHANNELS ; chan++)
    {
#if (sensorSelector_mux_MUXTYPE == sensorSelector_mux_MUX_SINGLE)
        sensorSelector_mux_Unset(chan);
#else
        sensorSelector_mux_CYAMUXSIDE_A_Unset(chan);
        sensorSelector_mux_CYAMUXSIDE_B_Unset(chan);
#endif
    }

    sensorSelector_mux_lastChannel = sensorSelector_mux_NULL_CHANNEL;
}


#if (!sensorSelector_mux_ATMOSTONE)
/*******************************************************************************
* Function Name: sensorSelector_mux_Select
********************************************************************************
* Summary:
*  This functions first disconnects all channels then connects the given
*  channel.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void sensorSelector_mux_Select(uint8 channel) 
{
    sensorSelector_mux_DisconnectAll();        /* Disconnect all previous connections */
    sensorSelector_mux_Connect(channel);       /* Make the given selection */
    sensorSelector_mux_lastChannel = channel;  /* Update last channel */
}
#endif


/*******************************************************************************
* Function Name: sensorSelector_mux_FastSelect
********************************************************************************
* Summary:
*  This function first disconnects the last connection made with FastSelect or
*  Select, then connects the given channel. The FastSelect function is similar
*  to the Select function, except it is faster since it only disconnects the
*  last channel selected rather than all channels.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void sensorSelector_mux_FastSelect(uint8 channel) 
{
    /* Disconnect the last valid channel */
    if( sensorSelector_mux_lastChannel != sensorSelector_mux_NULL_CHANNEL)
    {
        sensorSelector_mux_Disconnect(sensorSelector_mux_lastChannel);
    }

    /* Make the new channel connection */
#if (sensorSelector_mux_MUXTYPE == sensorSelector_mux_MUX_SINGLE)
    sensorSelector_mux_Set(channel);
#else
    sensorSelector_mux_CYAMUXSIDE_A_Set(channel);
    sensorSelector_mux_CYAMUXSIDE_B_Set(channel);
#endif


    sensorSelector_mux_lastChannel = channel;   /* Update last channel */
}


#if (sensorSelector_mux_MUXTYPE == sensorSelector_mux_MUX_DIFF)
#if (!sensorSelector_mux_ATMOSTONE)
/*******************************************************************************
* Function Name: sensorSelector_mux_Connect
********************************************************************************
* Summary:
*  This function connects the given channel without affecting other connections.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void sensorSelector_mux_Connect(uint8 channel) 
{
    sensorSelector_mux_CYAMUXSIDE_A_Set(channel);
    sensorSelector_mux_CYAMUXSIDE_B_Set(channel);
}
#endif

/*******************************************************************************
* Function Name: sensorSelector_mux_Disconnect
********************************************************************************
* Summary:
*  This function disconnects the given channel from the common or output
*  terminal without affecting other connections.
*
* Parameters:
*  channel:  The channel to disconnect from the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void sensorSelector_mux_Disconnect(uint8 channel) 
{
    sensorSelector_mux_CYAMUXSIDE_A_Unset(channel);
    sensorSelector_mux_CYAMUXSIDE_B_Unset(channel);
}
#endif

#if (sensorSelector_mux_ATMOSTONE)
/*******************************************************************************
* Function Name: sensorSelector_mux_DisconnectAll
********************************************************************************
* Summary:
*  This function disconnects all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void sensorSelector_mux_DisconnectAll(void) 
{
    if(sensorSelector_mux_lastChannel != sensorSelector_mux_NULL_CHANNEL) 
    {
        sensorSelector_mux_Disconnect(sensorSelector_mux_lastChannel);
        sensorSelector_mux_lastChannel = sensorSelector_mux_NULL_CHANNEL;
    }
}
#endif

/* [] END OF FILE */
