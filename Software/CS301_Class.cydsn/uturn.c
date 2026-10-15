#include "project.h"
#include "uturn.h"
#include "turn.h"
#include <stddef.h>

#define UTURN_SPEED             15
#define UTURN_MIN_COUNTS        150  // About 135 degrees: start looking here
#define UTURN_MAX_COUNTS        240  // Search limit, about 216 degrees
#define UTURN_ALIGN_SPEED       15
#define UTURN_ALIGN_CORRECTION   4
#define UTURN_CONFIRM_FRAMES     2
#define UTURN_TIMEOUT_MS      6000UL // Includes rotation, search and alignment
#define UTURN_SENSOR_TIMEOUT_MS 50UL

static UturnState state = UTURN_IDLE;
static int16_t startLeft;
static int16_t startRight;
static uint32_t startMs;
static uint32_t lastFrameMs;
static unsigned int alignedFrames;

static int32_t countsMoved(int16_t current, int16_t start)
{
    int32_t difference = (int32_t)current - (int32_t)start;
    if (difference > 32767) difference -= 65536;
    else if (difference < -32768) difference += 65536;
    if (difference < 0) difference = -difference;
    return difference;
}

static void FailTurn(void)
{
    SetMotorSpeed(0, 0);
    state = UTURN_FAULT;
}

bool UturnStart(uint32_t nowMs)
{
    if (state != UTURN_IDLE || TurnGetState() != TURN_IDLE) return false;
    startLeft = QuadDec_M1_GetCounter();
    startRight = QuadDec_M2_GetCounter();
    startMs = nowMs;
    lastFrameMs = nowMs;
    alignedFrames = 0;
    state = UTURN_ROTATE;
    SetMotorSpeed(UTURN_SPEED, -UTURN_SPEED);
    return true;
}

void UturnUpdate(const SensorFrame *frame, uint32_t nowMs)
{
    int32_t leftCounts;
    int32_t rightCounts;
    int fl, fr, rl, rr, correction;
    unsigned int i;

    if (state == UTURN_IDLE) return;
    if (state == UTURN_DONE || state == UTURN_FAULT) {
        SetMotorSpeed(0, 0);
        return;
    }
    if ((uint32_t)(nowMs - startMs) >= UTURN_TIMEOUT_MS || frame == NULL) {
        FailTurn();
        return;
    }
    if (frame->fresh) {
        for (i = 0; i < N_SENSORS; ++i) {
            if (frame->state[i] != SENSOR_BLACK &&
                frame->state[i] != SENSOR_WHITE) {
                FailTurn();
                return;
            }
        }
        lastFrameMs = nowMs;
    }
    if ((uint32_t)(nowMs - lastFrameMs) >= UTURN_SENSOR_TIMEOUT_MS) {
        FailTurn();
        return;
    }

    leftCounts = countsMoved(QuadDec_M1_GetCounter(), startLeft);
    rightCounts = countsMoved(QuadDec_M2_GetCounter(), startRight);

    // Bound rotation/search. Alignment has the overall time limit instead.
    if (state == UTURN_ROTATE || state == UTURN_FIND_LINE) {
        if (leftCounts >= UTURN_MAX_COUNTS || rightCounts >= UTURN_MAX_COUNTS) {
            FailTurn();
            return;
        }
    }
    if (state == UTURN_ROTATE) {
        SetMotorSpeed(UTURN_SPEED, -UTURN_SPEED);
        if (leftCounts >= UTURN_MIN_COUNTS && rightCounts >= UTURN_MIN_COUNTS) {
            state = UTURN_FIND_LINE;
        }
        return; // Start searching on a subsequent fresh sensor frame
    }
    if (!frame->fresh) return;

    fl = frame->state[Q6] == SENSOR_BLACK;
    fr = frame->state[Q5] == SENSOR_BLACK;
    rl = frame->state[Q1] == SENSOR_BLACK;
    rr = frame->state[Q2] == SENSOR_BLACK;

    if (state == UTURN_FIND_LINE) {
        // Keep rotating until both front middle sensors are on the line.
        if (!fl || !fr) {
            SetMotorSpeed(UTURN_SPEED, -UTURN_SPEED);
            return;
        }
        state = UTURN_ALIGN;
        alignedFrames = 0;
    }

    // Outer branch sensors do not steer the U-turn alignment.
    if (fl && fr && (rl || rr)) {
        ++alignedFrames;
        SetMotorSpeed(0, 0); // Hold while checking the next fresh frame
        if (alignedFrames >= UTURN_CONFIRM_FRAMES) state = UTURN_DONE;
        return;
    }
    alignedFrames = 0;

    if (!fl && !fr) {
        // Lost the front pair: resume searching in the original direction.
        state = UTURN_FIND_LINE;
        if (leftCounts >= UTURN_MAX_COUNTS || rightCounts >= UTURN_MAX_COUNTS) {
            FailTurn();
            return;
        }
        SetMotorSpeed(UTURN_SPEED, -UTURN_SPEED);
        return;
    }

    // Both front sensors black with rear white: advance gently.
    // A single front sensor black: steer toward that side of the line.
    correction = UTURN_ALIGN_CORRECTION * (fr - fl);
    SetMotorSpeed(UTURN_ALIGN_SPEED + correction,
                  UTURN_ALIGN_SPEED - correction);
}

UturnState UturnGetState(void)
{
    return state;
}

void UturnReset(void)
{
    if (state == UTURN_DONE || state == UTURN_FAULT) {
        SetMotorSpeed(0, 0);
        alignedFrames = 0;
        state = UTURN_IDLE;
    }
}
