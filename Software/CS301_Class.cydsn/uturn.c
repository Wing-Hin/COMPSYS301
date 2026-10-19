#include "project.h"
#include "uturn.h"
#include "turn.h"
#include <stdbool.h>
#include <stdint.h>

#define UTURN_SPEED  15
#define UTURN_COUNTS 200  // Adjust after testing

static int16_t startLeft;
static int16_t startRight;
static bool turning = false;

static int32_t countsMoved(int16_t current, int16_t start)
{
    int32_t difference = (int32_t)current - (int32_t)start;

    // Handle counter wraparound
    if (difference > 32767) {
        difference -= 65536;
    } else if (difference < -32768) {
        difference += 65536;
    }

    // Make the distance positive
    if (difference < 0) {
        difference = -difference;
    }

    return difference;
}

void UturnStart(void)
{
    if (turning) {
        return;
    }

    startLeft = QuadDec_M1_GetCounter();
    startRight = QuadDec_M2_GetCounter();

    turning = true;
    SetMotorSpeed(UTURN_SPEED, -UTURN_SPEED);
}

bool UturnUpdate(void)
{
    int32_t leftCounts;
    int32_t rightCounts;
    int leftSpeed = UTURN_SPEED;
    int rightSpeed = -UTURN_SPEED;

    // Stay stopped after the turn finishes
    if (!turning) {
        return false;
    }

    leftCounts = countsMoved(QuadDec_M1_GetCounter(), startLeft);
    rightCounts = countsMoved(QuadDec_M2_GetCounter(), startRight);

    if (leftCounts >= UTURN_COUNTS &&
        rightCounts >= UTURN_COUNTS) {
        SetMotorSpeed(0, 0);
        turning = false;
        return true;
    }

    // Stop each wheel when it reaches its target
    if (leftCounts >= UTURN_COUNTS) {
        leftSpeed = 0;
    }

    if (rightCounts >= UTURN_COUNTS) {
        rightSpeed = 0;
    }

    SetMotorSpeed(leftSpeed, rightSpeed);
    return false;
}