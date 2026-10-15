/* ========================================
 * Fully working code: 
 * PWM      : 
 * Encoder  : 
 * ADC      :
 * USB      : port displays speed and position.
 * CMD: "PW xx"
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ========================================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>
#include "line_follow.h"
#include "uturn.h"

#define UTURN_TEST_ON_START 1 // 1 = one U-turn at startup, then normal driving
#include "sensor.h"
#include "motorControl.h"

#include "robot_state.h"
#include "turn.h"
#include "turn_hardware.h"

//* ========================================
#include "defines.h"
#include "vars.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================

extern volatile int motorSpeed_tick;
extern volatile int Turn_isr_count;
volatile int count_beforeRestart;
volatile bool sideSensorDisable = 0;
volatile bool distanceReached=0;

SensorFrame f;// holds a snapshot of sensors. Is state fresh or not
RobotState currentState;//what robot line thing robot is detecting
RobotState previousState;//prev descion

/* SysTick callback slot 0 provides a real 1 ms clock for ADC-stall detection. */
static volatile uint32 lineFollowMs;//timer count ms
static uint32 lastLineFrameMs; //time of last fresh reading
static void LineFollowClock(void)//every SysTick counter increases
{
    ++lineFollowMs;
}// main loop can check how long it has been waiting for new data

static void UpdateLineFollowing(const SensorFrame *frame, uint32 now)
{
    if (frame->fresh) {//frame 
        lastLineFrameMs = now;
        straightFromFrame(25, frame); /* Keep the current base speed. */
    } else if ((uint32)(now - lastLineFrameMs) >=
               LINE_FOLLOW_SENSOR_TIMEOUT_MS) {
        straightFromFrame(0, NULL); /* Sensor stream has stopped updating. */
    }
}

void travel_for_m(float distance){
    int16_t left;
    int16_t right;
    int target_count;
    left = -QuadDec_M1_GetCounter();
    right = QuadDec_M2_GetCounter();
    target_count = (distance/(0.065 *PI))*228*1.03;
    if((((float)(left+right))/2) >= target_count){
        distanceReached=1;
    }
}

int main(void)
{
    bool startTest = UTURN_TEST_ON_START != 0;
    bool branchArmed = true;
    unsigned int clearFrames = 0;
    unsigned int i;
    bool valid;
    UturnState ustate;

    sensor_init();
    MotorInit();
    isr_TS_StartEx(isr_TS_Interrupt);
    MotorDisable();
    CYGlobalIntEnable;
#ifdef USE_USB
    USBUART_Start(0, USBUART_5V_OPERATION);
#endif
    RF_BT_SELECT_Write(0);
    CySysTickInit();
    CySysTickSetCallback(0U, LineFollowClock);
    CySysTickEnable();

    for (;;) {
        f = sensors_GetFrame();

        // Wait for valid sensors before the single startup test.
        if (startTest && f.fresh) {
            valid = true;
            for (i = 0; i < N_SENSORS; ++i) {
                if (f.state[i] != SENSOR_BLACK && f.state[i] != SENSOR_WHITE) {
                    valid = false;
                }
            }
            if (valid && UturnStart(lineFollowMs)) startTest = false;
        }

        ustate = UturnGetState();
        if (ustate != UTURN_IDLE) {
            // U-turn owns motors. Normal branches/turn updates are skipped.
            UturnUpdate(&f, lineFollowMs);
            Turn_isr_count = 0;
            if (UturnGetState() == UTURN_DONE) {
                UturnReset();
                lastLineFrameMs = lineFollowMs;
                branchArmed = false;
                clearFrames = 0;
                currentState = ROBOT_STATE_FOLLOW_LINE;
            }
            // FAULT stays stopped. Reset/reprogram to retry after inspection.
        } else if (!startTest) {
            if (TurnGetState() == TURN_IDLE) {
                if (f.fresh) {
                    previousState = currentState;
                    currentState = RobotDecideState(f.state);

                    // Re-arm only after leaving the previous branch/U-turn.
                    if (!branchArmed) {
                        if (currentState == ROBOT_STATE_FOLLOW_LINE &&
                            f.state[Q3] == SENSOR_WHITE && f.state[Q4] == SENSOR_WHITE) {
                            if (++clearFrames >= 3) branchArmed = true;
                        } else {
                            clearFrames = 0;
                        }
                    }
                    if (branchArmed && currentState == ROBOT_STATE_LEFT_BRANCH) {
                        if (TurnStart(TURN_LEFT)) {
                            branchArmed = false;
                            clearFrames = 0;
                        }
                    } else if (branchArmed && currentState == ROBOT_STATE_RIGHT_BRANCH) {
                        if (TurnStart(TURN_RIGHT)) {
                            branchArmed = false;
                            clearFrames = 0;
                        }
                    }
                }
                if (TurnGetState() == TURN_IDLE) {
                    if (currentState == ROBOT_STATE_SENSOR_FAULT || distanceReached) {
                        straightFromFrame(0, NULL);
                    } else if (!branchArmed ||
                               currentState == ROBOT_STATE_FOLLOW_LINE ||
                               currentState == ROBOT_STATE_LINE_LOST) {
                        UpdateLineFollowing(&f, lineFollowMs);
                    } else {
                        // No route choice supplied for a two-sided junction.
                        straightFromFrame(0, NULL);
                    }
                }
            }

            // Existing normal-turn scheduler; its ISR period is unchanged.
            if (Turn_isr_count >= 12) {
                Turn_isr_count = 0;
                TurnUpdate();
                if (TurnGetState() == TURN_DONE) {
                    if (++count_beforeRestart >= 10) {
                        count_beforeRestart = 0;
                        TurnReset();
                        lastLineFrameMs = lineFollowMs;
                    }
                } else {
                    count_beforeRestart = 0;
                }
            }
        }

        // This service must run during both normal motion and the U-turn.
        if (Motor_isr_flag == 1) {
            Motor_isr_flag = 0;
            Motor_captureRPM();
            Motor_maintainSpeed();
        }
        handle_usb();
        flag_KB_string = 0;
    }
}

void usbPutString(char *s)
{
#ifdef USE_USB
    uint16 length = 0;
    if (s == NULL || USBUART_CDCIsReady() == 0) return;
    while (length < 62 && s[length] != '\0') ++length;
    USBUART_PutData((uint8 *)s, length);
#endif
}

void usbPutChar(char c)
{
#ifdef USE_USB     
    if (USBUART_CDCIsReady() == 0) return;
    USBUART_PutChar(c);
#endif    
}
//* ========================================
void handle_usb()
{
    // handles input at terminal, echos it back to the terminal
    // turn echo OFF, key emulation: only CR
    // entered string is made available in 'line' and 'flag_KB_string' is set
    
    static uint8 usbStarted = FALSE;
    static uint16 usbBufCount = 0;
    uint8 c; 
    

    if (!usbStarted)
    {
        if (USBUART_GetConfiguration())
        {
            USBUART_CDC_Init();
            usbStarted = TRUE;
        }
    }
    else
    {
        if (USBUART_DataIsReady() != 0)
        {  
            c = USBUART_GetChar();

            if ((c == 13) || (c == 10))
            {
//                if (usbBufCount > 0)
                {
                    entry[usbBufCount]= '\0';
                    strcpy(line,entry);
                    usbBufCount = 0;
                    flag_KB_string = 1;
                }
            }
            else 
            {
                if (((c == CHAR_BACKSP) || (c == CHAR_DEL) ) && (usbBufCount > 0) )
                    usbBufCount--;
                else
                {
                    if (usbBufCount > (BUF_SIZE-2) ) // one less else strtok triggers a crash
                    {
                       usbPutChar('!');
                    }
                    else
                        entry[usbBufCount++] = c;  
                }  
            }
        }
    }    
}


/* [] END OF FILE */
