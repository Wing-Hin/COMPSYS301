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
#define ADC_FULL   4095


static volatile uint16 Vmax[N_SENSORS];
static volatile uint16 Vmin[N_SENSORS];
static volatile uint16 Vpp[N_SENSORS];
//static volatile uint8 state[N_SENSORS];
static volatile uint8 sampleCount;
//static volatile uint8 windowDone;
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
            if(pkpk > T_HIGH) {latest.state[i] = SENSOR_WHITE;}
            else if(pkpk < T_LOW) {latest.state[i] = SENSOR_BLACK;}
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