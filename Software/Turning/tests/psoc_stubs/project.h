/* Host compile-only declarations matching the PSoC APIs used by main/adapter.
 * These do not implement or emulate the PSoC peripherals.
 */
#ifndef TEST_PROJECT_H
#define TEST_PROJECT_H
#include <stdint.h>
typedef uint8_t uint8;
typedef uint32_t uint32;
#define CYGlobalIntEnable ((void)0)
void PWM_1_Start(void);
void PWM_2_Start(void);
void PWM_1_WriteCompare(uint8 value);
void PWM_2_WriteCompare(uint8 value);
uint8 PWM_1_ReadPeriod(void);
uint8 PWM_2_ReadPeriod(void);
void QuadDec_M1_Start(void);
void QuadDec_M2_Start(void);
int16_t QuadDec_M1_GetCounter(void);
int16_t QuadDec_M2_GetCounter(void);
void CySysTickInit(void);
void CySysTickEnable(void);
typedef void (*cySysTickCallback)(void);
cySysTickCallback CySysTickSetCallback(uint32 number, cySysTickCallback callback);
uint8 CyEnterCriticalSection(void);
void CyExitCriticalSection(uint8 state);
#endif
