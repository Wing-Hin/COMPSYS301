#ifndef MAIN_CONTROL_H
#define MAIN_CONTROL_H

#include "turn.h"

typedef enum {
    MAIN_STOPPED, MAIN_FOLLOWING, MAIN_TURNING, MAIN_FAULT
} MainControlState;

typedef enum {
    MAIN_FAULT_NONE, MAIN_FAULT_LINE_LOST, MAIN_FAULT_AMBIGUOUS_CORNER,
    MAIN_FAULT_TURN, MAIN_FAULT_SENSOR, MAIN_FAULT_SCHEDULER,
    MAIN_FAULT_REQUEST
} MainControlFault;

/* Call after hardware initialization and a valid, centred sensor frame.
 * An explicit restart is allowed after a controller fault only if the turn
 * module is IDLE/DONE/FAULT. A hardware fault during a moving turn requires
 * board reset. Never reset encoder hardware during a run.
 */
bool MainControlStart(void);

/* This is the ONLY motion-controller update main calls, once every 5 ms.
 * It calls TurnUpdate internally and exclusively owns both motor commands.
 * ReadSensor must use one valid cached sensor frame for the entire update.
 */
MainControlState MainControlUpdate5ms(void);
MainControlState MainControlGetState(void);
MainControlFault MainControlGetFault(void);
uint16_t MainControlGetCompletedTurns(void);

/* Latch a hardware/scheduling failure and stop. Subsequent updates hold stop.
 * Call from main context, not an ISR. No automatic restart after a fault.
 */
void MainControlHalt(MainControlFault reason);

#endif
