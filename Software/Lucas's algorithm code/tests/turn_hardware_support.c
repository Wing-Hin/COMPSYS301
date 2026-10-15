/* Adapt the older hardware test's encoder mock to today's motor API. */
#include <stdint.h>
int16_t QuadDec_M1_GetCounter(void);
int16_t QuadDec_M2_GetCounter(void);
void Motor_get_totalCount(int16_t *left, int16_t *right)
{ *left = QuadDec_M1_GetCounter(); *right = QuadDec_M2_GetCounter(); }
