#include <project.h>

#include "motorControl.h"
#include "sensor.h"

#define TURN_DIVISOR 2

void straight(uint8 speed){
    
    uint8 Q1_Left = getSingleSensorState(Q1);
    uint8 Q2_Right  = getSingleSensorState(Q2);

    // Both on white -> lost, stop and stay stopped
    if ((Q1_Left == SENSOR_WHITE) && (Q2_Right == SENSOR_WHITE)) {
        MotorDisable();
        return;
    }

    // Back on the line -> power motors up again
    if (!Motor_enabled) {
        MotorEnable();
    }

    if ((Q1_Left == SENSOR_BLACK) && (Q2_Right == SENSOR_BLACK)) {
        // Both on line
        MotorLeft_setRPM(speed);
        MotorRight_setRPM(speed);

    } else if ((Q1_Left == SENSOR_WHITE) && (Q2_Right == SENSOR_BLACK)) {
        // Right sensor off the line
        MotorLeft_setRPM(speed);
        MotorRight_setRPM(speed-2);

    } else if ((Q1_Left == SENSOR_BLACK) && (Q2_Right == SENSOR_WHITE)) {
        // Left sensor off the line
        MotorLeft_setRPM(speed-2);
        MotorRight_setRPM(speed);
    }else if ((Q1_Left == SENSOR_WHITE) && (Q2_Right == SENSOR_WHITE)){
        MotorLeft_setRPM(0);
        MotorRight_setRPM(0);
    }
    else{
    
}
    
}