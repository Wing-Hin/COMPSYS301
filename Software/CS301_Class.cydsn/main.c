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
#include "sensor.h"
#include "motorControl.h"

#include "robot_state.h"
#include "turn.h"
#include "turn_hardware.h"
#include "branchSearch.h"
#include "bfs.h"
#include "map.h"
#include "uturn.h"

//* ========================================
#include "defines.h"
#include "vars.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================

//
//motorcontrol
extern volatile int motorSpeed_tick;
volatile int count_beforeRestart;
volatile int count_beforeNextPath;
volatile bool sideSensorDisable = 0;
volatile bool distanceReached=0;
//
//sensor and state
SensorFrame f;
RobotState currentState;
RobotState previousState;
//

/* SysTick callback slot 0 provides a real 1 ms clock for ADC-stall detection. */
static volatile uint32 lineFollowMs;
static uint32 lastLineFrameMs;
static void LineFollowClock(void)
{
    ++lineFollowMs;
}

static void UpdateLineFollowing(const SensorFrame *frame, uint32 now)
{
    if (frame->fresh) {
        lastLineFrameMs = now;
        straightFromFrame(18, frame); /* Keep the current base speed. */
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

int main()
{
    

// --------------------------------    
// ----- INITIALIZATIONS ----------
    sensor_init();
    MotorInit();
    isr_TS_StartEx(isr_TS_Interrupt);
    MotorDisable(); /* Wait for a valid frame before enabling movement. */
    //MotorLeft_setRPM(30);
    //MotorRight_setRPM(30);
    BfsWorkspace workspace;
    BfsCell path[BFS_MAX_CELLS];
    uint16_t pathLength = 0;
    BfsCell start = {0, 0};
    BfsCell food[2] = {{5, 9},{0,0}};
    int foodNUM = 1;
    BfsCell pre_cell;
    BfsCell now_cell;
    BfsCell next_cell;
    Direction_Turned direction;
    BfsStatus status = BFS_NO_PATH;
    JunctionList *junctions;
    float init_count = 0;
    float current_count = 0;
    bool turned = 1;
    bool foodGrabed=0;
    bool uturnEnd = 1;
    
    
    const uint8_t *selectedMap = maps[MAP_TEST];

    int junctionList_i = 0;
    int junctionIgnore = 0;
    
    CYGlobalIntEnable;
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);

    CySysTickInit(); /* Existing CyLib uses a nominal 1 ms tick. */
    CySysTickSetCallback(0U, LineFollowClock);
    CySysTickEnable();

    //usbPutString(displaystring);
    
    for(;;)
    {   
        if(UturnUpdate()){
            uturnEnd = 1;
        }
        //distance calculation
        if(turned && TurnGetState() == TURN_IDLE){
            init_count = Motor_get_AveCount();
            turned = 0;
        }
        current_count = Motor_get_AveCount();
        //Path finding
        if(status == BFS_NO_PATH){
            pre_cell = path[pathLength-2];
            now_cell = path[pathLength-1];
            if(foodNUM == 1){
                status = BfsFindPath(selectedMap, 15, 19, start, food[0], 
                            &workspace, path, BFS_MAX_CELLS, &pathLength);
            }
            else{
                status = BfsFindPath(selectedMap, 15, 19, food[foodNUM-2], food[foodNUM-1], 
                            &workspace, path, BFS_MAX_CELLS, &pathLength);
            }
            next_cell = path[1];
        }
        if (status == BFS_FOUND){
            if(getTurnDirection(pre_cell,now_cell,next_cell,&direction)){
                if(direction == LEFT){
                    TurnStart(TURN_LEFT);
                }
                else if (direction == RIGHT){
                    TurnStart(TURN_RIGHT);
                }
                else if (direction == BACK){
                    UturnStart();
                    uturnEnd = 0;
                }
            }
            junctions = junctionsDetections(selectedMap, path, pathLength);
            status = BFS_INVALID_INPUT;
            junctionList_i = 0;
            junctionIgnore = 0;
        }
        //
        f =  sensors_GetFrame();
        if(junctionList_i >= (*junctions).count && TurnGetState() == TURN_IDLE){
            if((current_count - init_count) >= (((float)(*junctions).numOfNodesToGoal)*0.09*288*1.2/(0.065*PI))){
                straightFromFrame(0, NULL);
                if(foodGrabed==0){
                    LED_1_Write(1);
                    LED_2_Write(1);
                    LED_3_Write(1);
                    LED_4_Write(1);
                    LED_5_Write(1);
                    LED_6_Write(1);
                }
                foodGrabed =1;
            }
            else{
                UpdateLineFollowing(&f, lineFollowMs);
            }
            
        }
        else if(uturnEnd){
        //Sensors LEDs
        if(f.fresh){
            LED_1_Write(getSingleSensorState(Q1) == SENSOR_WHITE);
            LED_2_Write(getSingleSensorState(Q2) == SENSOR_WHITE);
            LED_3_Write(f.state[Q3] == SENSOR_WHITE);
            LED_4_Write(f.state[Q4] == SENSOR_WHITE);
            LED_5_Write(f.state[Q5] == SENSOR_WHITE);
            LED_6_Write(f.state[Q6] == SENSOR_WHITE);
        }
        //
        //Turn decision
        previousState = currentState;
        currentState = RobotDecideState(f.state);
        //If left branch detects, and ignored enough branches
        //If should turn left, trun left and set i++ and ignore to 0
        if(previousState == ROBOT_STATE_FOLLOW_LINE && currentState == ROBOT_STATE_LEFT_BRANCH && TurnGetState() == TURN_IDLE){
            if(junctionIgnore >= (*junctions).items[junctionList_i].numOfJunctionsIgnore&&
            ((current_count - init_count) >= ((float)(*junctions).items[junctionList_i].numOfNode)*0.09*288*1.2/(0.065*PI))){
                if((*junctions).items[junctionList_i].turn == LEFT){
                    TurnStart(TURN_LEFT);
                    turned = 1;
                    junctionList_i++;
                    junctionIgnore = 0;
                }
            }
            //otherwise ignore this we wants to turn left
            else{
                if((*junctions).items[junctionList_i].turn == LEFT){
                    junctionIgnore++;
                }
                UpdateLineFollowing(&f, lineFollowMs);
            }
        }
        //If right branch detects, and ingored enough brances
        //If it should turn right, turn right and set i++ and ignore to 0
        else if(previousState == ROBOT_STATE_FOLLOW_LINE && currentState == ROBOT_STATE_RIGHT_BRANCH && TurnGetState() == TURN_IDLE){
            if(junctionIgnore >= (*junctions).items[junctionList_i].numOfJunctionsIgnore&&
            ((current_count - init_count) >= ((float)(*junctions).items[junctionList_i].numOfNode)*0.09*288*1.2/(0.065*PI))){
                if((*junctions).items[junctionList_i].turn == RIGHT){
                    TurnStart(TURN_RIGHT);
                    turned = 1;
                    junctionList_i++;
                    junctionIgnore = 0;
                }
            }
            //otherwise ignore this
            else{
                if((*junctions).items[junctionList_i].turn == RIGHT){
                    junctionIgnore++;
                }
                UpdateLineFollowing(&f, lineFollowMs);
            }
        }
        //If junction detects, and enough junctions ignored
        //decide which way to turn, and set i++ and ingore to 0
        else if ((previousState == ROBOT_STATE_LEFT_BRANCH || previousState == ROBOT_STATE_RIGHT_BRANCH)&& 
                    currentState == ROBOT_STATE_JUNCTION && TurnGetState() == TURN_IDLE){
            if(junctionIgnore >= (*junctions).items[junctionList_i].numOfJunctionsIgnore&&
                ((current_count - init_count) >= ((float)(*junctions).items[junctionList_i].numOfNode)*0.09*288*1.2/(0.065*PI))){
                if((*junctions).items[junctionList_i].turn == LEFT){
                    TurnStart(TURN_LEFT);
                    turned = 1;
                    junctionList_i++;
                    junctionIgnore = 0;
                }
                else if((*junctions).items[junctionList_i].turn == RIGHT){
                    TurnStart(TURN_RIGHT);
                    turned = 1;
                    junctionList_i++;
                    junctionIgnore = 0;
                }
            }
            //Otherwise keep going. 
            //Junction ignored based on previous state
            else{
                if (previousState == ROBOT_STATE_LEFT_BRANCH){
                    if((*junctions).items[junctionList_i].turn == RIGHT){
                        junctionIgnore++;
                    }
                }
                else if (previousState == ROBOT_STATE_RIGHT_BRANCH){
                    if((*junctions).items[junctionList_i].turn == LEFT){
                        junctionIgnore++;
                    }
                }
                UpdateLineFollowing(&f, lineFollowMs);
            }
        }
        //
        else if ((currentState == ROBOT_STATE_FOLLOW_LINE && TurnGetState() == TURN_IDLE)||
                (currentState == ROBOT_STATE_LINE_LOST && TurnGetState() == TURN_IDLE)){
            if(distanceReached == 0){
                UpdateLineFollowing(&f, lineFollowMs);
            }
        }
        }
        //
                /*
        travel_for_m(1);
        if(distanceReached){
            MotorLeft_setRPM(0);
            MotorRight_setRPM(0);
            MotorDisable();
        }
                */
        /* Reuse the LED snapshot: reading sensors_GetFrame again would clear
         * or consume freshness independently. Do not call this during a turn
         * when turn integration is added; that controller must own the motors.
         */
        handle_usb();
        
            flag_KB_string = 0;
        if(Motor_isr_flag ==1){
            Motor_isr_flag = 0;
            Motor_captureRPM();
            Motor_maintainSpeed();
            if(foodGrabed==1)
            {
                LED_1_Write(!LED_1_Read());
                LED_2_Write(!LED_2_Read());
                LED_3_Write(!LED_3_Read());
                LED_4_Write(!LED_4_Read());
                LED_5_Write(!LED_5_Read());
                LED_6_Write(!LED_6_Read());
                count_beforeNextPath++;
            }
            if(count_beforeNextPath >= 30){
                foodGrabed=0;
                junctionList_i=0;
                if(foodNUM<2){
                    foodNUM++;
                }
                status = BFS_NO_PATH;
                count_beforeNextPath = 0;
            }
        }
        
        if(Turn_isr_count >=12){
            Turn_isr_count=0;
            TurnUpdate();
            if(TurnGetState() == TURN_DONE){
                count_beforeRestart++;
                if(count_beforeRestart >=10){
                    count_beforeRestart = 0;
                    TurnReset();
                }
            }
            
        }
        
        
    }        
}   
//* ========================================
//* ========================================
void usbPutString(char *s)
{
// !! Assumes that *s is a string with allocated space >=64 chars     
//  Since USB implementation retricts data packets to 64 chars, this function truncates the
//  length to 62 char (63rd char is a '!')

#ifdef USE_USB     
    /* Do not let a disconnected USB host block the sensor timeout. */
    if (USBUART_CDCIsReady() == 0) return;
    s[63]='\0';
    s[62]='!';
    USBUART_PutData((uint8*)s,strlen(s));
#endif
}
//* ========================================
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
