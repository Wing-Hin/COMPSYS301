#ifndef TEST_MOTOR_H
#define TEST_MOTOR_H
#include "project.h"
#include <stdbool.h>
#define Forward true
#define Backward false
void MotorEnable(void);
void MotorDisable(void);
void MotorLeft_setDirection(bool direction);
void MotorRight_setDirection(bool direction);
#endif
