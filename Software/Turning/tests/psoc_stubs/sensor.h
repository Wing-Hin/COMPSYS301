#ifndef TEST_SENSOR_H
#define TEST_SENSOR_H
#include "project.h"
#define N_SENSORS 6
#define Q1 0
#define Q2 1
#define Q3 2
#define Q4 3
#define Q5 4
#define Q6 5
#define SENSOR_BLACK 0
#define SENSOR_WHITE 1
#define SENSOR_UNKNOWN 2
typedef struct { uint8 state[N_SENSORS]; uint8 fresh; } SensorFrame;
void sensor_init(void);
SensorFrame sensors_GetFrame(void);
#endif
