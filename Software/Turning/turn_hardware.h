#ifndef TURN_HARDWARE_H
#define TURN_HARDWARE_H

#include "sensor.h"

/* Main refreshes this once per tick using the existing sensors_GetFrame().
 * ADC interrupts update the driver's own frame, not this copy. Every sensor
 * read in one controller update therefore sees the same six-channel sample.
 */
extern SensorFrame controlSensorFrame;

#endif
