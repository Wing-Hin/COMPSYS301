/* Exercise the actual turn controller's new cancellation API. */
#include "turn.h"
#include <assert.h>
#include <stdio.h>
static int16_t leftCommand, rightCommand;
void Motor_reset_totalCount(void) {}
void SetMotorSpeed(int16_t left, int16_t right)
{ leftCommand = left; rightCommand = right; }
void ReadWheelEncoderCounts(int16_t *left, int16_t *right)
{ *left = 0; *right = 0; }
uint8_t ReadSensor(SensorId sensor) { (void)sensor; return 1; }
int main(void)
{
    assert(TurnStart(TURN_LEFT));
    assert(leftCommand > 0 && rightCommand > 0);
    TurnAbort();
    assert(TurnGetState() == TURN_FAULT);
    assert(leftCommand == 0 && rightCommand == 0);
    assert(TurnUpdate() == TURN_FAULT);
    assert(!TurnStart(TURN_RIGHT));
    TurnReset();
    assert(TurnGetState() == TURN_IDLE);
    assert(TurnStart(TURN_RIGHT));
    TurnAbort();
    puts("PASS: real turn controller abort, fault latch and explicit restart");
    return 0;
}
