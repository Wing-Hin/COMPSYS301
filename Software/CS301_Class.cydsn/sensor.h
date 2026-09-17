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

typedef struct {
    int16 black;    // calibrated dark level  B_i
    int16 white;    // calibrated bright level W_i
    int16 t_low;    // T_i - H_i
    int16 t_high;   // T_i + H_i
    uint8 state;    // last decision, 0 or 1 — hysteresis needs it
} sensor_t;

sensor_t sensor[N_SENSORS];

uint8 isSensorOnWhite(uint8 sensor_th);

/* [] END OF FILE */
