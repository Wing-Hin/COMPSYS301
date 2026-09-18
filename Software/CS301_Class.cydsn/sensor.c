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



uint8 isSensorOnWhite(uint8 sensor_th){
    uint8 Vmax_white = 0;
    uint8 Vmin_white = 0;
    uint8 Vmax_black = 0;
    uint8 Vmin_black = 0;
    
    CYGlobalIntEnable; /* Enable Global interrupts */
    ADC_Sensor_Start();
    ADC_Sensor_IRQ_Enable(); /* Enable ADC interrupts */
    
    sensor_t sensor; // create sensor data container
    
    
}