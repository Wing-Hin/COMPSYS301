#include <project.h>
#include "motorControl.h"
#include "sensor.h"
#define TURN_DIVISOR 2


    

void straight(int speed){

    uint8 Q1_Right = getSingleSensorState(0);
    uint8 Q2_Left = getSingleSensorState(1);


    if ((Q1_Right == 0) && (Q2_Left == 0)) {
        // Both on line
        MotorLeft_setRPM(speed);
        MotorRight_setRPM(speed);



    }else if ((Q1_Right == 1) && (Q2_Left == 0)) {
        // Right on line but left is going to the left too much
        MotorLeft_setRPM(speed);
        MotorRight_setRPM(speed/TURN_DIVISOR);
        


    }else if ((Q1_Right == 0) && (Q2_Left == 1)) {
        // left on line but left is going to the right too much
        MotorLeft_setRPM(speed/TURN_DIVISOR);
        MotorRight_setRPM(speed);
        


    }else if ((Q1_Right == 1) && (Q2_Left == 1)){
    //Both on White 
    //lost?
        

    }    
}
