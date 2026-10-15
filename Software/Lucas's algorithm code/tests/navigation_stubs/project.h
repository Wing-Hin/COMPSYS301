#ifndef NAV_TEST_PROJECT_H
#define NAV_TEST_PROJECT_H
#include "../../../CS301_Class.cydsn/tests/line_follow_stubs/project.h"
int16_t QuadDec_M1_GetCounter(void);
int16_t QuadDec_M2_GetCounter(void);
void isr_TS_Interrupt(void);
void isr_TS_StartEx(void (*handler)(void));
#endif
