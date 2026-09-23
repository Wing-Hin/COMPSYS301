#include "turn.h"
#include "turn_hardware.h"
#include "motorControl.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Mock the canonical motorControl.c interface, not the PWM peripherals. */
volatile bool Motor_isr_flag;
volatile bool Motor_enabled;

static int leftTarget;
static int rightTarget;
static bool leftDirection;
static bool rightDirection;
static bool leftStarted;
static bool rightStarted;
static unsigned enableCalls;
static unsigned disableCalls;
static unsigned leftDirectionCalls;
static unsigned rightDirectionCalls;
static unsigned sequence;
static unsigned leftStopOrder;
static unsigned rightStopOrder;
static unsigned leftDirectionOrder;
static unsigned rightDirectionOrder;
static int16_t encoderLeft;
static int16_t encoderRight;

void MotorEnable(void)
{
    ++enableCalls;
    Motor_enabled = true;
    leftStarted = true;
    rightStarted = true;
}

void MotorDisable(void)
{
    ++disableCalls;
    Motor_enabled = false;
    leftStarted = false;
    rightStarted = false;
}

void MotorLeft_setRPM(int speed) { leftTarget = speed; }
void MotorRight_setRPM(int speed) { rightTarget = speed; }

void MotorLeft_stop(void)
{
    leftStarted = false;
    leftStopOrder = ++sequence;
}

void MotorRight_stop(void)
{
    rightStarted = false;
    rightStopOrder = ++sequence;
}

void MotorLeft_start(void)
{
    leftStarted = true;
    ++sequence;
}

void MotorRight_start(void)
{
    rightStarted = true;
    ++sequence;
}

void MotorLeft_setDirection(bool direction)
{
    leftDirection = direction;
    ++leftDirectionCalls;
    leftDirectionOrder = ++sequence;
}

void MotorRight_setDirection(bool direction)
{
    rightDirection = direction;
    ++rightDirectionCalls;
    rightDirectionOrder = ++sequence;
}

int16_t QuadDec_M1_GetCounter(void) { return encoderLeft; }
int16_t QuadDec_M2_GetCounter(void) { return encoderRight; }

static void ResetOrder(void)
{
    sequence = 0;
    leftStopOrder = 0;
    rightStopOrder = 0;
    leftDirectionOrder = 0;
    rightDirectionOrder = 0;
}

static void TestSensorMapping(void)
{
    static const uint8 channels[SENSOR_COUNT] = {Q3, Q6, Q5, Q4, Q1, Q2};
    unsigned logical;
    unsigned other;

    for (logical = 0; logical < SENSOR_COUNT; ++logical) {
        for (other = 0; other < N_SENSORS; ++other) {
            controlSensorFrame.state[other] = SENSOR_WHITE;
        }
        controlSensorFrame.state[channels[logical]] = SENSOR_BLACK;
        for (other = 0; other < SENSOR_COUNT; ++other) {
            assert(ReadSensor((SensorId)other) == (other == logical ? 1U : 0U));
        }
    }
}

static void TestMotorCommands(void)
{
    unsigned directionsBefore;

    Motor_enabled = false;
    SetMotorSpeed(0, 0);
    assert(leftTarget == 0 && rightTarget == 0);
    assert(!Motor_enabled && disableCalls == 1);

    SetMotorSpeed(25, -40);
    assert(leftTarget == 25 && rightTarget == 40);
    assert(leftDirection && !rightDirection); /* Forward, Backward. */
    assert(leftStarted && rightStarted && Motor_enabled);
    assert(enableCalls == 1);

    /* A repeated command must not toggle direction or disable either wheel. */
    directionsBefore = leftDirectionCalls + rightDirectionCalls;
    ResetOrder();
    SetMotorSpeed(25, -40);
    assert(leftStopOrder == 0 && rightStopOrder == 0);
    assert(leftDirectionCalls + rightDirectionCalls == directionsBefore);
    assert(enableCalls == 1);

    /* Reversal stops each wheel before changing its direction. */
    ResetOrder();
    SetMotorSpeed(-30, 40);
    assert(leftStopOrder != 0 && rightStopOrder != 0);
    assert(leftStopOrder < leftDirectionOrder);
    assert(rightStopOrder < rightDirectionOrder);
    assert(!leftDirection && rightDirection);
    assert(leftStarted && rightStarted);

    /* Commands are bounded before being passed to motorControl.c. */
    SetMotorSpeed(200, -200);
    assert(leftTarget == 100 && rightTarget == 100);

    SetMotorSpeed(0, -55);
    assert(leftTarget == 0 && rightTarget == 55);
    assert(!leftStarted && rightStarted && Motor_enabled);

    SetMotorSpeed(0, 0);
    assert(!Motor_enabled && !leftStarted && !rightStarted);
    assert(leftTarget == 0 && rightTarget == 0);

    SetMotorSpeed(-10, 0);
    assert(Motor_enabled && leftStarted && !rightStarted);
    assert(!leftDirection && leftTarget == 10 && rightTarget == 0);
    assert(enableCalls == 2);
}

static void TestEncoderRead(void)
{
    int16_t left;
    int16_t right;
    encoderLeft = -1234;
    encoderRight = 2345;
    ReadWheelEncoderCounts(&left, &right);
    assert(left == encoderLeft && right == encoderRight);
}

int main(void)
{
    memset(&controlSensorFrame, 0, sizeof controlSensorFrame);
    TestSensorMapping();
    TestMotorCommands();
    TestEncoderRead();
    puts("PASS: hardware adapter uses motorControl targets, enables and directions.");
    return 0;
}
