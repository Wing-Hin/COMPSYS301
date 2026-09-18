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

/* [] END OF FILE */

#include <project.h>
#include "sensor.h"

uint16 calibratedBlack = 123; //temp assigned a value before the calibration function created.
uint16 calibratedWhite = 123;

CY_ISR_PROTO(sensorReader_isr);
CY_ISR(sensorReader_isr){
    
}


uint8 isSensorOnWhite(uint8 sensor_th){
    uint8 Vmax_white = 0;
    uint8 Vmin_white = 0;
    uint8 Vmax_black = 0;
    uint8 Vmin_black = 0;
    
    CYGlobalIntEnable; /* Enable Global interrupts */
    
    sensorSelector_mux_Start();
    sensorSelector_mux_FastSelect(sensor_th);
    
    isr_1_Start();
    ADC_Sensor_Start();
    ADC_Sensor_IRQ_Enable(); /* Enable ADC interrupts */
    
    sensor_t sensor; // create sensor data 
    sensor.black = calibratedBlack;
    sensor.white = calibratedWhite;
    
    ADC_Sensor_StartConvert();
    
    
    
}