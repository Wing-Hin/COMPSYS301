#include <project.h>
#include "motorControl.h"
#include "sensor.h"
#include "line_follow.h"
#define TURN_DIVISOR 2
main {



    void function(int speed){

        uint8 Q1_Right = isSensorOnWhite(0);
        uint8 Q2_Left = isSensorOnWhite(1);


        }if (Q1_Right == 0) && (Q2_Left == 0) {
            // Both on line
            MotorLeft_SetRPM(speed);
            MotorRight_SetRPM(speed);



        }else if (Q1_Right == 1) && (Q2_Left == 0) {
            // Right on line but left is going to the left too much
            MotorLeft_SetRPM(speed);
            MotorRight_SetRPM(speed/TURN_DIVISOR);
            


        }else if (Q1_Right == 0) && (Q2_Left == 1x) {
     // left on line but left is going to the right too much
            MotorLeft_SetRPM(speed/TURN_DIVISOR);
            MotorRight_SetRPM(speed);
            


        }else{
                    if (Q1_Right == 1) && (Q2_Left == 1) then {
//Both on White 
//lost?
            

        }
        
    }
}