#include "line_follow.h"
#include "motorControl.h"
#include <stddef.h>

#define LF_LOST_MAX  125 // frames to search when all four are white 125 x 8 ms = 1 s, then stop       

/* ---- Lookup-table steering (LF_USE_TABLE 1) ---- */
#define LF_STEP      1   /* speed points per level: level 3 -> +3 on outer wheel */

#define LF_MEM_F   100   /* front pair lost: steer 3 toward last front side     */
#define LF_MEM_R   101   /* rear pair lost: counter-steer 1 from last rear side */
#define LF_LOST    102   /* all four white: search toward last side seen        */

/* Index = FL*8 + FR*4 + RL*2 + RR, 1 = black (FL=Q6, FR=Q5, RL=Q1, RR=Q2).
 * + steers right, - steers left. Edit any entry to change how the robot
 * reacts to that exact pattern; keep mirror entries equal and opposite
 * (13/14, 7/11, 6/9, 5/10, 4/8, 1/2).
 */
static const int8 LF_LEVEL[16] = {
    /*  0  WW/WW  everything lost          */ LF_LOST,
    /*  1  WW/WB  front lost, line right   */  3,
    /*  2  WW/BW  front lost, line left    */ -3,
    /*  3  WW/BB  front lost, rear centred */ LF_MEM_F,
    /*  4  WB/WW  front drift left, rear lost */ 2,
    /*  5  WB/WB  shifted left, parallel   */  1,
    /*  6  WB/BW  angled strongly left     */  5,
    /*  7  WB/BB  front drifting left      */  3,
    /*  8  BW/WW  front drift right, rear lost */ -2,
    /*  9  BW/WB  angled strongly right    */ -5,
    /* 10  BW/BW  shifted right, parallel  */ -1,
    /* 11  BW/BB  front drifting right     */ -3,
    /* 12  BB/WW  front centred, rear lost */ LF_MEM_R,
    /* 13  BB/WB  rear drifted left        */ -2,
    /* 14  BB/BW  rear drifted right       */  2,
    /* 15  BB/BB  centred and straight     */  0,
};

/* ---- KP/KD formula steering (LF_USE_TABLE 0) ---- */
#define LF_KP        3   /* position gain: pulls robot back onto the line */
#define LF_KD        5.5 /* heading gain: the damping, stops the swing    */
#define LF_MAX_CORR  2   /* cap in speed-% points: base 10 -> 14/10 max   */

static void StopFollowing(void)
{
    /* Clear previous targets so a later enable cannot reuse an old request. */
    MotorLeft_setRPM(0);
    MotorRight_setRPM(0);
    MotorDisable();
}

static int ClampSpeed(int speed)
{
    if (speed < 0) return 0;
    if (speed > 99) return 99;
    return speed;
}


void straightFromFrame(uint8 speed, const SensorFrame *frame)
{
    static const uint8 channels[4] = {Q6, Q5, Q1, Q2};
    static int lastFront, lastRear;   /* last side each pair saw the line: +1 right, -1 left */
    static unsigned int lostFrames;   /* frames in a row with all four white */
    unsigned int i;
    int fl, fr, rl, rr, boost, left, right;

    if (frame == NULL || speed == 0) { StopFollowing(); return; }
    for (i = 0; i < 4; ++i) {
        uint8 s = frame->state[channels[i]];
        if (s != SENSOR_BLACK && s != SENSOR_WHITE) { StopFollowing(); return; }
    }

    fl = frame->state[Q6] == SENSOR_BLACK;
    fr = frame->state[Q5] == SENSOR_BLACK;
    rl = frame->state[Q1] == SENSOR_BLACK;
    rr = frame->state[Q2] == SENSOR_BLACK;

    if (!fl && !fr && !rl && !rr) {
        /* All four white: keep steering toward the last side seen for up to
         * LF_LOST_MAX frames. Stop if the line was never seen or the search
         * runs out.
         */
        if ((lastFront == 0 && lastRear == 0) || ++lostFrames > LF_LOST_MAX) {
            StopFollowing();
            return;
        }
    } else {
        lostFrames = 0;
    }


    int level;

    /* Remember which side each pair last saw the line on. */
    if (fl != fr) lastFront = fr - fl;
    if (rl != rr) lastRear  = rr - rl;
    int8 context = fl * 8 + fr * 4 + rl * 2 + rr;
    level = LF_LEVEL[context]; // read sensors in binrary access
    if (level == LF_MEM_F) {
        level = 8 * lastFront;
    } else if (level == LF_MEM_R) {
        level = -lastRear;
    } else if (level == LF_LOST) {
        level = 8 * (lastFront != 0 ? lastFront : lastRear);
    }
    boost = LF_STEP * level;

    /* Steer by speeding up the outer wheel only; the inner wheel stays at
     * base speed so it never drops into the motor's stall region.
     */
    if (boost > 0) {                  /* turn right: left wheel faster */
        if(context == (int8)6){
            left  = ClampSpeed((int)speed);
            right = ClampSpeed((int)speed*0.5);
        }else if(context == (int8)8){
            right  = ClampSpeed((int)speed);
            left = ClampSpeed((int)speed*0.3);
        }
        else{
            left  = ClampSpeed((int)speed + boost);
            right = ClampSpeed((int)speed);
        }

    } else {                          /* turn left: right wheel faster */
        if(context == (int8)9){
            right  = ClampSpeed((int)speed);
            left = ClampSpeed((int)speed*0.5);
        }else if(context == (int8)4){
            left  = ClampSpeed((int)speed);
            right = ClampSpeed((int)speed*0.3);
        }else{
             left  = ClampSpeed((int)speed);
            right = ClampSpeed((int)speed - boost);
        }

    }

    MotorLeft_setRPM(left);
    MotorRight_setRPM(right);
    MotorLeft_setDirection(Forward);
    MotorRight_setDirection(Forward);
    if (!Motor_enabled) MotorEnable();
    /* speed > 0 here and steering only adds, so both wheels always run. */
    MotorLeft_start();
    MotorRight_start();
}

void straight(uint8 speed)
{
    /* Legacy wrapper for callers that have not already consumed a frame. */
    SensorFrame frame = sensors_GetFrame();
    if (speed == 0 || frame.fresh) straightFromFrame(speed, &frame);
}
