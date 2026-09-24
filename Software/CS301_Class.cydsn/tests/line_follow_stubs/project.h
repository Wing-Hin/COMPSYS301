#ifndef LINE_FOLLOW_TEST_PROJECT_H
#define LINE_FOLLOW_TEST_PROJECT_H
#include <stdint.h>
typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef void (*cySysTickCallback)(void);
#define CYGlobalIntEnable ((void)0)
#define USBUART_5V_OPERATION 0U
void CySysTickInit(void);
void CySysTickEnable(void);
cySysTickCallback CySysTickSetCallback(uint32 slot, cySysTickCallback callback);
void RF_BT_SELECT_Write(uint8 value);
void USBUART_Start(uint8 device, uint8 mode);
uint8 USBUART_CDCIsReady(void);
void USBUART_PutData(const uint8 *data, uint16 length);
void USBUART_PutChar(char c);
uint8 USBUART_GetConfiguration(void);
void USBUART_CDC_Init(void);
uint8 USBUART_DataIsReady(void);
uint8 USBUART_GetChar(void);
void LED_1_Write(uint8 value);
void LED_2_Write(uint8 value);
void LED_3_Write(uint8 value);
void LED_4_Write(uint8 value);
void LED_5_Write(uint8 value);
void LED_6_Write(uint8 value);
#endif
