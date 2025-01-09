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
#define ARROW_RIGHT_U   335
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
// ~335 : B+D (problematic, need to prevent B+D output C)
// ~680 : A
// ~680 : A+Any other key

// Issues: same as above 
// Note: it seems the value can be same as above
// But, this is reverse for further refine 
#define ABCD_D_U    100 
#define ABCD_D_L    200
#define ABCD_C_L    300 
#define ABCD_C_U    335
#define ABCD_B_L    470
#define ABCD_B_U    510
#define ABCD_A_L    660
#define ABCD_A_U    700

//ABCD mode idc:
// it is found that when using inspire3d_display, the value is not stable
// maybe ws2812b_simple.h is the cause
// so different value is used
// values(D2,D3,D2+D3)
// 162 433->555 595-717 : A
// 237 378->476 615-713 : B
// 70  235->330 305-400 : C
// 163 197->254 360-417 : D
// 42  140-158  182-200 : No key pressed

#define IDC_ABCD_HVKEY_L_COM    280 // Have keys or not, use combine value
#define IDC_ABCD_B_L_D2         230 // B use D2
#define IDC_ABCD_B_U_D2         280 // B use D2
#define IDC_ABCD_C_L_D2         58  // C use D2
#define IDC_ABCD_C_U_D2         78  // C use D2
#define IDC_ABCD_A_L_D2         150 // A use D2, combine value
#define IDC_ABCD_A_U_D2         200 // A use D2, combine value
#define IDC_ABCD_A_L_COM        590 // A: combine value
#define IDC_ABCD_A_U_COM        730 // A: combine value
#define IDC_ABCD_D_L_D2         150 // D use D2, combine value
#define IDC_ABCD_D_U_D2         200 // D use D2, combine value
#define IDC_ABCD_D_L_COM        350 // D: combine value
#define IDC_ABCD_D_U_COM        430 // D: combine value


