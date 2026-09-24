/* Compile the real main as well, to test its shared-frame timeout helper. */
#define main firmware_main
#include "../main.c"
#undef main
#include <assert.h>

volatile bool Motor_enabled;
volatile bool Motor_isr_flag;
static SensorFrame frame;
static int leftTarget, rightTarget;
static bool leftOn, rightOn, leftForward, rightForward;
static unsigned reads;

void MotorInit(void) {}
void MotorEnable(void) { Motor_enabled = true; leftOn = rightOn = true; }
void MotorDisable(void) { Motor_enabled = false; leftOn = rightOn = false; }
void MotorLeft_setRPM(int x) { leftTarget = x; }
void MotorRight_setRPM(int x) { rightTarget = x; }
void MotorLeft_setDirection(bool x) { leftForward = x; }
void MotorRight_setDirection(bool x) { rightForward = x; }
void MotorLeft_start(void) { leftOn = true; }
void MotorRight_start(void) { rightOn = true; }
void MotorLeft_stop(void) { leftOn = false; }
void MotorRight_stop(void) { rightOn = false; }
void Motor_captureRPM(void) {}
void Motor_maintainSpeed(void) {}
void sensor_init(void) {}
SensorFrame sensors_GetFrame(void) { ++reads; return frame; }
void CySysTickInit(void) {}
void CySysTickEnable(void) {}
cySysTickCallback CySysTickSetCallback(uint32 n, cySysTickCallback cb)
{ (void)n; (void)cb; return NULL; }
void RF_BT_SELECT_Write(uint8 x) { (void)x; }
void USBUART_Start(uint8 a, uint8 b) { (void)a; (void)b; }
uint8 USBUART_CDCIsReady(void) { return 0; }
void USBUART_PutData(const uint8 *p, uint16 n) { (void)p; (void)n; }
void USBUART_PutChar(char c) { (void)c; }
uint8 USBUART_GetConfiguration(void) { return 0; }
void USBUART_CDC_Init(void) {}
uint8 USBUART_DataIsReady(void) { return 0; }
uint8 USBUART_GetChar(void) { return 0; }
void LED_1_Write(uint8 x) { (void)x; }
void LED_2_Write(uint8 x) { (void)x; }
void LED_3_Write(uint8 x) { (void)x; }
void LED_4_Write(uint8 x) { (void)x; }
void LED_5_Write(uint8 x) { (void)x; }
void LED_6_Write(uint8 x) { (void)x; }

/* Pattern bits: front-left, front-right, rear-left, rear-right. */
static void Pattern(unsigned mask)
{
    const uint8 channels[] = {Q6, Q5, Q1, Q2};
    unsigned i;
    for (i = 0; i < N_SENSORS; ++i) frame.state[i] = SENSOR_UNKNOWN;
    for (i = 0; i < 4; ++i)
        frame.state[channels[i]] = (mask & (1U << i)) ? SENSOR_BLACK : SENSOR_WHITE;
    frame.fresh = 1;
}

int main(void)
{
    const int expected[16][2] = {
        {0,0}, {8,12}, {12,8}, {10,10},
        {9,11}, {17,23}, {21,19}, {19,21},
        {11,9}, {19,21}, {23,17}, {21,19},
        {10,10}, {18,22}, {22,18}, {20,20}
    };
    const uint8 channels[] = {Q6, Q5, Q1, Q2};
    unsigned mask, speed, i;
    int originalLeft, originalRight;

    for (mask = 0; mask < 16; ++mask) {
        Pattern(mask);
        straightFromFrame(20, &frame);
        assert(leftTarget == expected[mask][0] && rightTarget == expected[mask][1]);
        assert(Motor_enabled == (mask != 0));
        if (mask) assert(leftForward && rightForward);
    }
    /* Sweep every input speed: valid outputs and mirror symmetry. */
    for (speed = 0; speed <= 255; ++speed) {
        for (mask = 0; mask < 16; ++mask) {
            Pattern(mask);
            straightFromFrame((uint8)speed, &frame);
            assert(leftTarget >= 0 && leftTarget <= 99);
            assert(rightTarget >= 0 && rightTarget <= 99);
            assert(leftOn == (leftTarget != 0));
            assert(rightOn == (rightTarget != 0));
            if (speed == 0) assert(!Motor_enabled);
            originalLeft = leftTarget;
            originalRight = rightTarget;
            Pattern(((mask & 5U) << 1) | ((mask & 10U) >> 1));
            straightFromFrame((uint8)speed, &frame);
            assert(leftTarget == originalRight && rightTarget == originalLeft);
        }
    }
    for (i = 0; i < 4; ++i) {
        Pattern(15);
        straightFromFrame(20, &frame);
        frame.state[channels[i]] = SENSOR_UNKNOWN;
        straightFromFrame(20, &frame);
        assert(!Motor_enabled && leftTarget == 0 && rightTarget == 0);
        frame.state[channels[i]] = 255;
        straightFromFrame(20, &frame);
        assert(!Motor_enabled);
    }
    straightFromFrame(20, NULL);
    assert(!Motor_enabled && !leftOn && !rightOn);

    Pattern(15);
    UpdateLineFollowing(&frame, 100);
    assert(Motor_enabled && reads == 0); /* No second sensor-frame read. */
    assert(leftTarget == 10 && rightTarget == 10); /* Existing main speed. */
    frame.fresh = 0;
    UpdateLineFollowing(&frame, 149);
    assert(Motor_enabled);
    UpdateLineFollowing(&frame, 150);
    assert(!Motor_enabled && leftTarget == 0 && rightTarget == 0);
    Pattern(15);
    UpdateLineFollowing(&frame, 151);
    assert(Motor_enabled); /* Valid fresh data resumes following. */
    lastLineFrameMs = UINT32_MAX - 20;
    frame.fresh = 0;
    UpdateLineFollowing(&frame, 29);
    assert(!Motor_enabled); /* Timeout works across clock wrap. */
    Pattern(15);
    straight(20);
    assert(reads == 1 && Motor_enabled);
    frame.fresh = 0;
    straight(0);
    assert(!Motor_enabled);
    usbPutString(displaystring); /* Unavailable USB must not block. */
    usbPutChar('x');
    puts("four-sensor line following: all tests passed");
    return 0;
}
