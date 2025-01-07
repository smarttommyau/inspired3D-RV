#pragma once

#include "funconfig.h"


// Arrow Key values:(C4)
// ~32? : no key pressed
// ~160 : down
// ~328 : right
// ~220 : down + right
// ~496 : up 
// ~685 : left

// All combined key would be dropped due to inconsistent of the values of combine key input

// Lower bound : End with L
// Upper bound : End with U 
// Chosen by human intuition
// Reference by testing data as above
// Bound choosen size may varies due to voltage instability and human error
// May run testing program to recalibrate

#define ARROW_DOWN_L    100
#define ARROW_DOWN_U    200 
#define ARROW_RIGHT_L   300 
#define ARROW_RIGHT_U   350
#define ARROW_UP_L      470
#define ARROW_UP_U      510
#define ARROW_LEFT_L    660
#define ARROW_LEFT_U    700

// ABCD key values:(D2)
// ~32? : no key pressed
// ~160 : D
// ~220 : C+D
// <=326 : C
// ~430-500: B+C
// ~496 : B
// ~335 : B+D
// ~680 : A
// ~680 : A+Any other key

// Issues: same as above 
// Note: it seems the value can be same as above
// But, this is reverse for further refine 
#define ABCD_D_U    100 
#define ABCD_D_L    200
#define ABCD_C_L    300 
#define ABCD_C_U    350
#define ABCD_B_L    470
#define ABCD_B_U    510
#define ABCD_A_L    660
#define ABCD_A_U    700