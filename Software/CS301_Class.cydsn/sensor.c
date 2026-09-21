/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/



#include <project.h>
#include "sensor.h"

// These number is based on Ross's experiement.
#define T_PKPK    737      // 0.9 V 
#define T_HYST     98      // 0.12 V, gap/4
#define T_HIGH   (T_PKPK + T_HYST)
#define T_LOW    (T_PKPK - T_HYST)
#define N_SAMPLES  20

static volatile uint16 Vmax;
static volatile uint16 Vmin;

static volatile uint8 sampleCount;
static volatile uint8 windowDone;



CY_ISR_PROTO(eocHandler);
CY_ISR(eocHandler){
    Timer_sensor_ReadStatusRegister();
    
    int16 v = ADC_Sensor_GetResult16();
    if (v < 0) v = 0;
    
    if (v > Vmax) Vmax = v;
    if( v < Vmin) Vmin = v;
    if(++sampleCount >= N_SAMPLES){
        Timer_sensor_Stop();
        windowDone = 1;
    }
    
}


void sensor_init(void) {
    sensorSelector_mux_Start();
    ADC_Sensor_Start();
    isr_1_StartEx(eocHandler);
}


uint8 isSensorOnWhite(uint8 sensor_th){
    
    // initialisation ------------------------
    
    Vmax = 0;
    Vmin = 4095;
    sampleCount = 0;
    windowDone  = 0;
    
    sensorSelector_mux_FastSelect(sensor_th);
    
    // ----------------------------------------

    Timer_sensor_Start(); // trigger the chain
    
    while(!windowDone){} // Wait for sampling finishs
    
    uint16 f = Vmax - Vmin;
    if(f > T_HIGH) return 1;
    else if (f < T_LOW) return 0;
    else return 2; // within the band
        
}

/* [] END OF FILE */