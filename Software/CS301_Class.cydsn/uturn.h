#ifndef UTURN_H
#define UTURN_H
#include "sensor.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    UTURN_IDLE,
    UTURN_ROTATE,
    UTURN_FIND_LINE,
    UTURN_ALIGN,
    UTURN_DONE,
    UTURN_FAULT
} UturnState;

bool UturnStart(uint32_t nowMs);
void UturnUpdate(const SensorFrame *frame, uint32_t nowMs);
UturnState UturnGetState(void);
void UturnReset(void);
#endif
