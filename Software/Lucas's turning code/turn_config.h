#ifndef TURN_CONFIG_H
#define TURN_CONFIG_H

/* EDIT THE VALUES IN THIS FILE TO TUNE THE ROBOT.
 * Speeds are 0..100 percent commands passed to the existing motor driver's
 * setRPM functions; they are not raw PWM values or literal RPM. Keep the
 * #ifndef guards so tests can override them. Defaults are not calibration.
 *
 * Common settings:
 *   TURN_APPROACH_SPEED   forward driver command, 1..100 percent
 *   TURN_ROTATE_SPEED     rotating driver command, 1..100 percent
 *   TURN_APPROACH_COUNTS  forward distance in counts for EACH wheel
 *   TURN_CONFIRM_READINGS consecutive checks before a transition, at least 2
 *   TURN_TIMEOUT_MS       total approach + rotation limit in milliseconds
 *   encoder signs         +1 or -1 independently for forward wheel motion
 */
#ifndef TURN_APPROACH_SPEED
#define TURN_APPROACH_SPEED 20
#endif
#ifndef TURN_ROTATE_SPEED
#define TURN_ROTATE_SPEED 25
#endif
/* Forward travel from the front intersection trigger to the desired pivot.
 * 34 counts is ONLY an initial estimate (~30 mm using the existing driver's
 * 57*4 counts/revolution and 65 mm wheel diameter). Calibrate on the robot.
 * counts = distance_mm * counts_per_wheel_revolution / (pi * diameter_mm).
 */
#ifndef TURN_APPROACH_COUNTS
#define TURN_APPROACH_COUNTS 31L
#endif
/* Set each to +1 or -1 so a forward-moving wheel accumulates positive travel.
 * Encoder count polarity is independent of the motor command polarity.
 */
#ifndef TURN_LEFT_ENCODER_FORWARD_SIGN
#define TURN_LEFT_ENCODER_FORWARD_SIGN 1
#endif
#ifndef TURN_RIGHT_ENCODER_FORWARD_SIGN
#define TURN_RIGHT_ENCODER_FORWARD_SIGN 1
#endif
#ifndef TURN_TIMEOUT_MS
#define TURN_TIMEOUT_MS 400000UL
#endif
#ifndef TURN_CONFIRM_READINGS
#define TURN_CONFIRM_READINGS 2U
#endif
/* Scheduling contract: call the controller every 5 ms; do not tune this. */
#define TURN_UPDATE_MS 2UL

#endif
