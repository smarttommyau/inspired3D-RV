// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
#include "ch32v003_i2c.h"
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

void show_color_pallete(Inspire3D_Display * display){
    Inspire3D_Display_Reset(display);
    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 5; y++){
            for(int z = 0; z < 5;z++){
                uint8_t index = Inspire3D_Display_Coords2Index(x,y,z);
                Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
            }
        }
    }
    Inspire3D_Display_Update(display);
}


char display_buffer[sizeof(Inspire3D_Display)];
int main(void) {
    SystemInit();
    GPIO_ADCinit();
    i2c_init();
    i2c_scan();

    printf("I2C initialized\n");
    // int seed = 0;
    int x,y,z = 0;// selector coords
    Inspire3D_Display * display = (Inspire3D_Display *)&display_buffer;
    Inspire3D_Display_Init(display,GPIOA, PA2);
    Inspire3D_Display_Clear(display);// reset data and update
    Inspire3D_Display_SetBrightness(display, 0.05);
    show_color_pallete(display);
    while(1){
        // read ABCD key
        uint16_t abcd_reading = abcd_key_read_ADC();
        uint16_t arrow_reading = arrow_key_read_ADC();
        ARROW_KEY arrow = arrow_key_down(arrow_reading);
        if(arrow == ARROW_UP){
            y = (y + 1) % 5;
        } else if(arrow == ARROW_DOWN){
            y = (y - 1) % 5;
        } else if(arrow == ARROW_RIGHT){
            x = (x + 1) % 5;
        } else if(arrow == ARROW_LEFT){
            x = (x - 1) % 5;
        }
        // read arrow key
        if(abcd_reading >= IDC_ABCD_A_L_D2 && abcd_reading <= IDC_ABCD_A_U_D2){
            z = (z + 1) % 5;
        } else if(abcd_reading >= IDC_ABCD_B_L_D2 && abcd_reading <= IDC_ABCD_B_U_D2){
            z = (z - 1) % 5;
        }
        // set color
        uint8_t index = Inspire3D_Display_Coords2Index(x,y,z);
        // Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
        printf("x: %d, y: %d, z: %d adc: %d\n", x, y, z, abcd_reading);
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Black);
        Inspire3D_Display_Update(display);
        Delay_Ms(100);
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
        Inspire3D_Display_Update(display);
        Delay_Ms(100);
    }
}
