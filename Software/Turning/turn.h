#ifndef TURN_H
#define TURN_H

#include <stdbool.h>
#include <stdint.h>

/* All commonly changed values are grouped in this file. */
#include "turn_config.h"

/* Configuration checks: invalid settings stop compilation. */
#if TURN_APPROACH_SPEED < 1 || TURN_APPROACH_SPEED > 100
#error "TURN_APPROACH_SPEED must be 1..100"
#endif
#if TURN_ROTATE_SPEED < 1 || TURN_ROTATE_SPEED > 100
#error "TURN_ROTATE_SPEED must be 1..100"
#endif
#if TURN_APPROACH_COUNTS < 1 || TURN_APPROACH_COUNTS > 1000000L
#error "TURN_APPROACH_COUNTS must be 1..1000000 counts per wheel"
#endif
#if (TURN_LEFT_ENCODER_FORWARD_SIGN != 1 && TURN_LEFT_ENCODER_FORWARD_SIGN != -1) || \
    (TURN_RIGHT_ENCODER_FORWARD_SIGN != 1 && TURN_RIGHT_ENCODER_FORWARD_SIGN != -1)
#error "Encoder forward signs must each be +1 or -1"
#endif
#if TURN_CONFIRM_READINGS < 2 || TURN_CONFIRM_READINGS > 65535U
#error "TURN_CONFIRM_READINGS must be 2..65535"
#endif
#if TURN_TIMEOUT_MS < TURN_UPDATE_MS || TURN_TIMEOUT_MS > 4294967295UL
#error "TURN_TIMEOUT_MS must fit uint32_t and be at least 5 ms"
#endif
#if (TURN_TIMEOUT_MS % TURN_UPDATE_MS) != 0
#error "TURN_TIMEOUT_MS must be a multiple of 5 ms"
#endif

typedef enum {
    TURN_LEFT,
    TURN_RIGHT
} TurnDirection;

typedef enum {
    TURN_IDLE,
    TURN_APPROACH,
    TURN_LEAVE_LINE,
    TURN_FIND_LINE,
    TURN_DONE,
    TURN_FAULT
} TurnState;

/* Logical positions only: these values are NOT ADC channel numbers.
 * Front:  FL  FM_L  FM_R  FR
 * Back:      BM_L  BM_R
 * There are no rear outer sensors. BM_L/BM_R cannot detect axle arrival at
 * a branch: both already see the incoming line when the robot is centred.
 */
typedef enum {
    SENSOR_FL, SENSOR_FM_L, SENSOR_FM_R, SENSOR_FR,
    SENSOR_BM_L, SENSOR_BM_R,
    SENSOR_COUNT
} SensorId;

/* Main calls once per junction. Only IDLE accepts a valid direction.
 * A successful start immediately commands slow forward motion.
 */
bool TurnStart(TurnDirection direction);

/* Call exactly once every 5 ms, from the same execution context as Start/Reset.
 * IDLE never reads sensors or writes motors. Every other state owns motors.
 * Main must suppress other target-command writers until Reset returns to IDLE.
 * The existing motor driver's periodic capture/regulator service must continue.
 */
TurnState TurnUpdate(void);
TurnState TurnGetState(void);

/* Acknowledge DONE/FAULT and release ownership. Ignored while busy.
 * Main must handle the result BEFORE calling this; there is no auto-restart.
 */
void TurnReset(void);

/* Sensor and motor connections -- implement in your hardware adapter.
 * See turn_hardware.c for the adapter using existing project APIs.
 * Reads must be non-blocking, valid, and normalized: black=1, background=0.
 * Read both front middle sensors from a coherent sample. Do polarity
 * conversion here, never in the state machine.
 */
uint8_t ReadSensor(SensorId sensor);

/* Signed percentages -100..+100: positive=forward, negative=reverse, 0=stop.
 * The PSoC adapter passes abs(speed) to MotorLeft/Right_setRPM() and uses the
 * sign for direction. It must never pass a negative target to motorControl.c.
 * This function only updates targets/enables and must remain non-blocking.
 */
void SetMotorSpeed(int16_t left, int16_t right);

/* Third hardware connection for the selected encoder-distance approach.
 * Return the existing free-running signed 16-bit quadrature counts. Do not
 * reset counters: the normal controller may also use them. Both encoders must
 * already be started. Read promptly without waiting for movement. Each wheel
 * must move fewer than 32768 counts between 5 ms updates to resolve wraparound.
 */
void ReadWheelEncoderCounts(int16_t *left, int16_t *right);

#endif
