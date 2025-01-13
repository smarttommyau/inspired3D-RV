// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
// #include "ch32v003_i2c.h"
#include "inspire3d_display.h"

/// coords to led index
// Back layer
// 0,5,10,15,20
// 1,6,11,16,21
// 2,7,12,17,22
// 3,8,13,18,23
// 4,9,14,19,24

// Front layer
// 100,105,110,115,120
// 101,106,111,116,121
// 102,107,112,117,122
// 103,108,113,118,123
// 104,109,114,119,124

// Values of the led
// {g,r,b}

char display_buffer[sizeof(Inspire3D_Display)];
int main(void) {
    SystemInit();
    // GPIO_ADCinit();
    MY_I2C_init();
    // i2c_init();
    // i2c_scan();

    printf("I2C initialized\n");
    int seed = 0;
    Inspire3D_Display * display = (Inspire3D_Display *)&display_buffer;
    Inspire3D_Display_Init(display,GPIOA, PA2);
    Inspire3D_Display_Clear(display);// reset data and update
    Inspire3D_Display_SetBrightness(display, display->brightness);
    while(1){
        seed++;
        JOY_setseed(seed);
        Inspire3D_Display_Reset(display); // reset data to 0
        uint8_t index = Inspire3D_Display_Coords2Index(JOY_random()%4,JOY_random()%4,JOY_random()%4);
        Inspire3D_Display_SetColor(display,index, Inspire3D_Color_Red);
        Inspire3D_Display_Update(display);
        printf("updated LED %d\n", index);
        Delay_Ms(1000);
    }
}
