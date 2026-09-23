#include <project.h>

#include "motorControl.h"
#include "sensor.h"

#define TURN_DIVISOR 4

void straight(uint8 speed){
    
    uint8 Q1_Right = getSingleSensorState(1);
    uint8 Q2_Left  = getSingleSensorState(2);

    // Both on white -> lost, stop and stay stopped
    if ((Q1_Right == SENSOR_WHITE) && (Q2_Left == SENSOR_WHITE)) {
        MotorDisable();
        return;
    }

    // Back on the line -> power motors up again
    if (!Motor_enabled) {
        MotorEnable();
    }

    if ((Q1_Right == SENSOR_BLACK) && (Q2_Left == SENSOR_BLACK)) {
        // Both on line
        MotorLeft_start();
    MotorRight_start();
        MotorLeft_setRPM(speed);
        MotorRight_setRPM(speed);

    } else if ((Q1_Right == SENSOR_WHITE) && (Q2_Left == SENSOR_BLACK)) {
        // Right sensor off the line
        MotorLeft_start();
        MotorRight_stop();
        MotorLeft_setRPM(speed);
        MotorRight_setRPM(speed/TURN_DIVISOR);

    } else if ((Q1_Right == SENSOR_BLACK) && (Q2_Left == SENSOR_WHITE)) {
        // Left sensor off the line
        MotorLeft_stop();
        MotorRight_start();
        MotorLeft_setRPM(speed / TURN_DIVISOR);
        MotorRight_setRPM(speed);
    }else if ((Q1_Right == SENSOR_WHITE) && (Q2_Left == SENSOR_WHITE)){
        MotorLeft_stop();
        MotorRight_stop();
     MotorLeft_setRPM(0);
     MotorRight_setRPM(0);
    }
    else{
    
}
    
}