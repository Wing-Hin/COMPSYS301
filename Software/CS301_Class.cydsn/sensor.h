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

#define N_SENSORS 6
#define Q1 0
#define Q2 1
#define Q3 2
#define Q4 3
#define Q5 4
#define Q6 5

#define SENSOR_BLACK   0
#define SENSOR_WHITE   1
#define SENSOR_UNKNOWN 2 

typedef struct {
    uint8  state[N_SENSORS];   // SENSOR_WHITE / BLACK / UNKNOWN
    uint8  fresh;              //1 if a new window finished since the last call 
} SensorFrame;

void resetWindow(void);
void sensor_init(void);
SensorFrame sensors_GetFrame(void);
uint8 getSingleSensorState(int8 sensor_th);

/* [] END OF FILE */
