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

static const uint16 T_MID[N_SENSORS] = {550, 482, 636, 790, 502, 510};
static const uint16 T_HYS[N_SENSORS] = { 79,  60,  93, 132,  72,  70};
#define N_SAMPLES  20
#define ADC_FULL   4095


static volatile uint16 Vmax[N_SENSORS];
static volatile uint16 Vmin[N_SENSORS];
static volatile uint16 Vpp[N_SENSORS];
static volatile uint8 sampleCount;
static volatile SensorFrame latest;



CY_ISR_PROTO(eocHandler);
CY_ISR(eocHandler){
    uint16 i;
    
    for (i = 0; i < N_SENSORS; i++) {
        int16 v = ADC_Sensor_GetResult16(i);   
        if (v < 0) v = 0;
        if (v > Vmax[i]) Vmax[i] = v;   
        if (v < Vmin[i]) Vmin[i] = v;
    }
    
    if(++sampleCount >= N_SAMPLES){
        for( i = 0 ; i < N_SENSORS ; i++){
            uint16 pkpk = Vmax[i] - Vmin[i];
            Vpp[i] = pkpk; // becase Vpp is volatile, to avoid loading value from RAM many times, here use a variable f.
            if(pkpk > T_MID[i] + T_HYS[i] ) {latest.state[i] = SENSOR_WHITE;}
            else if(pkpk < T_MID[i] - T_HYS[i] ) {latest.state[i] = SENSOR_BLACK;}
        } 
        latest.fresh = 1;
        resetWindow();
        
    }

}
//	call every 8 ms, from the ISR
void resetWindow(){
    uint8 i;
    for(i = 0 ; i < N_SENSORS ; i++){
        Vmax[i] = 0;
        Vmin[i] = ADC_FULL;
    }
    sampleCount = 0;
}

// setup the sensor function at startup.
void sensor_init(void) {
    uint8 i;
    for(i = 0 ; i < N_SENSORS ; i++){
        latest.state[i] = SENSOR_UNKNOWN;
        Vpp[i] = 0;
    }
    resetWindow();
 
    ADC_Sensor_Start();
    ADC_Sensor_IRQ_StartEx(eocHandler);   // internal IRQ of the sequencer
    ADC_Sensor_StartConvert();            //free running starts from here 
}

SensorFrame sensors_GetFrame(void){
    SensorFrame sensor_frame_copy;
    uint8 s = CyEnterCriticalSection();
    sensor_frame_copy = *(SensorFrame *)&latest;
    latest.fresh = 0;
    CyExitCriticalSection(s);
    
    return sensor_frame_copy;
}

uint8 getSingleSensorState(int8 sensor_th){
    if(sensor_th > N_SENSORS || sensor_th ==0 ) return SENSOR_UNKNOWN;
    return latest.state[sensor_th];
}

/* [] END OF FILE */