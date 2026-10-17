#include "line_follow.h"
#include "motorControl.h"
#include <stddef.h>

/* TUNING: targets are speed-percentage points, NOT PWM duty or degrees.
 * Positive correction turns RIGHT; negative turns LEFT.
 * left = base + correction; right = base - correction.
 * Start with these values, then tune on the physical robot.
 */
#define LF_STEERING_GAIN       1
#define LF_MAX_CORRECTION     10
#define LF_MIN_WHEEL_SPEED    10 /* Starting floor; verify on your robot. */
#define LF_FRONT_LOST_CORR     8
#define LF_REAR_LOST_CORR      2
#define LF_SEARCH_CORRECTION  10
#define LF_LOST_MAX_FRAMES    50 /* 400 ms ONLY if fresh frames are 8 ms apart. */

/* Pattern bits: FL FR RL RR = Q6 Q5 Q1 Q2; B=1, W=0.
 * Entries are correction points BEFORE LF_STEERING_GAIN.
 * Patterns 0, 3, 12 use remembered direction below, not these zeros.
 * Keep mirrored pairs equal and opposite when tuning.
 */
static const int8 correctionTable[16] = {
    /*  0 WW/WW: search using memory */       0,
    /*  1 WW/WB: line at rear right */        8,
    /*  2 WW/BW: line at rear left */        -8,
    /*  3 WW/BB: front lost; use memory */    0,
    /*  4 WB/WW: line at front right */       3,
    /*  5 WB/WB: line on right side */        2,
    /*  6 WB/BW: angled; steer right */       2, // Reduced +4 to +2
    /*  7 WB/BB: front line on right */       1, // Reduced +3 to +1
    /*  8 BW/WW: line at front left */       -3,
    /*  9 BW/WB: angled; steer left */       -2, // Reduced -4 to -2
    /* 10 BW/BW: line on left side */        -2,
    /* 11 BW/BB: front line on left */       -1, // Reduced -3 to -1
    /* 12 BB/WW: rear lost; use memory */     0,
    /* 13 BB/WB: rear right; countersteer */ -1, // Reduced -2 to -1
    /* 14 BB/BW: rear left; countersteer */   1, // Reduced +2 to +1
    /* 15 BB/BB: centred */                  0
};

static int lastFront = 0;
static int lastRear = 0;
static unsigned int lostFrames = 0;

static void StopFollowing(void)
{
    MotorLeft_setRPM(0);
    MotorRight_setRPM(0);
    MotorDisable();
    lastFront = 0;
    lastRear = 0;
    lostFrames = 0;
}

void straightFromFrame(uint8 speed, const SensorFrame *frame)
{
    static const uint8 channels[4] = {Q6, Q5, Q1, Q2};
    unsigned int i;
    int fl, fr, rl, rr, pattern, correction;
    int base, floor, limit, direction;

    if (frame == NULL || speed == 0) {
        StopFollowing();
        return;
    }
    for (i = 0; i < 4; ++i) {
        if (frame->state[channels[i]] != SENSOR_BLACK &&
            frame->state[channels[i]] != SENSOR_WHITE) {
            StopFollowing();
            return;
        }
    }
    /* Caller (main.c) owns the stale-frame timeout and turn arbitration. */
    if (!frame->fresh) return;

    fl = frame->state[Q6] == SENSOR_BLACK;
    fr = frame->state[Q5] == SENSOR_BLACK;
    rl = frame->state[Q1] == SENSOR_BLACK;
    rr = frame->state[Q2] == SENSOR_BLACK;
    pattern = fl * 8 + fr * 4 + rl * 2 + rr;

    if (fl != fr) lastFront = fr - fl;
    if (rl != rr) lastRear = rr - rl;

  if (pattern == 0) {
    if (lastFront != 0) {
        direction = lastFront;
    } else {
        direction = lastRear;
    }

    lostFrames++;

    if (direction == 0 || lostFrames > LF_LOST_MAX_FRAMES) {
        StopFollowing();
        return;
    }

    correction = LF_SEARCH_CORRECTION * direction;
} else {
    lostFrames = 0;

    if (pattern == 3) {
        correction = LF_FRONT_LOST_CORR * lastFront;
    } else if (pattern == 12) {
        correction = -LF_REAR_LOST_CORR * lastRear;
    } else {
        correction = correctionTable[pattern] * LF_STEERING_GAIN;
    }
}
    /* Limit correction symmetrically: keep the average target at base,
     * keep both wheels forward, and keep targets within 0..99.
     * Below the configured floor, honour the caller's slower base speed;
     * correction becomes zero rather than silently increasing speed.
     */
    base = speed;
    if (base > 99) base = 99;
    floor = LF_MIN_WHEEL_SPEED;
    if (floor > base) floor = base;
    limit = LF_MAX_CORRECTION;
    if (limit > base - floor) limit = base - floor;
    if (limit > 99 - base) limit = 99 - base;
    if (correction > limit) correction = limit;
    if (correction < -limit) correction = -limit;

    MotorLeft_setRPM(base + correction);
    MotorRight_setRPM(base - correction);
    MotorLeft_setDirection(Forward);
    MotorRight_setDirection(Forward);
    if (!Motor_enabled) MotorEnable();
    MotorLeft_start();
    MotorRight_start();
}

void straight(uint8 speed)
{
    SensorFrame frame = sensors_GetFrame();

    if (speed == 0 || frame.fresh) {
        straightFromFrame(speed, &frame);
    }
}