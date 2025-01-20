// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
// #define WAIT_WS_CONNECT // wait for websocket connection before starting for emulator
// #defin CLOSE_WS_ON_CLOSE // close emulator when connection is closed
#include "driver.h"
#include "inspire3d_display.h"

//comment each part if not needed


///////////////////////
//      Float        //
///////////////////////
// round float to int
// only work for positive number
int my_round(float x)
{
    return (int)(x + 0.5);
}


///////////////////////
// Inspire3D Display //
///////////////////////
char display_buffer[sizeof(Inspire3D_Display)];
Inspire3D_Display * display = (Inspire3D_Display *)&display_buffer;



///////////////////////
//       Setup       //
///////////////////////

void setup(){
    SystemInit();//setup system
    GPIO_ADCinit();//setup ADC for Buttons
// add this if needed #define WAIT_WS_CONNECT // wait for websocket connection before starting for emulator
// use the above only when you find out the emulator is not updating the initial display
    MY_I2C_init();//setup I2C for 3d display

    ///// Setup Inspired3D Display
    Inspire3D_Display_Init(display,GPIOA, PA2);
    Inspire3D_Display_Clear(display);
    // uncomment below to set brightness
    // Inspire3D_Display_SetBrightness(display, 1);
    ///// Float printing
    // there are significant precision loss when printing float
    // There are no float printing in printf
    printf("Brightness:  %d.%d\n", (int)display->brightness, (int)((display->brightness - (int)display->brightness)*100));


    ///// set seed for random
    // note: you may get a seed number by repeatly add 1 to seed
    // then, wait for a button to start the program
    // to achieve a better random number
    JOY_setseed(0x1234);
    // sample wait for button press
    // uint16_t seed = 0;
    // while(1){
    //     ABCD_KEY abcd = abcd_key_down( abcd_key_read_ADC());
    //     ARROW_KEY arrow = arrow_key_down(arrow_key_read_ADC());
    //     if(abcd != ABCD_NOT_FOUND ||  arrow != ARROW_NOT_FOUND){
    //         break;
    //     }
    //     seed++;
    //     if(seed >= 65530){
    //         seed = 0;
    //     }
    //     Delay_Ms(10);
    // }
    // JOY_setseed(seed);

}

void Loop(){
    ////// sample button detection 
    ARROW_KEY arrow = arrow_key_down(arrow_key_read_ADC());
    ABCD_KEY abcd   = abcd_key_down(abcd_key_read_ADC());
    switch(arrow){
        case ARROW_UP:
            // do something
            break;
        case ARROW_DOWN:
            // do something
            break;
        case ARROW_LEFT:
            // do something
            break;
        case ARROW_RIGHT:
            // do something
            break;
        default:
            break;
    }
    switch(abcd){
        case ABCD_A:
            // do something
            break;
        case ABCD_B:
            // do something
            break;
        case ABCD_C:
            // do something
            break;
        case ABCD_D:
            // do something
            break;
        default:
            break;
    }
    // Add delay to debounce
    // Delay_Ms(100);

    ///// sample display update
    // set whole display to green
    Inspire3D_Display_SetBGColor(display, Inspire3D_Color_Green);
    // set a pixel(0,0,0) to red
    Inspire3D_Display_SetColor(display, Inspire3D_Display_Coords2Index(0,0,0), Inspire3D_Color_Red);
    // custom rgb color
    Inspire3D_Display_SetColor(display, Inspire3D_Display_Coords2Index(1,1,1), Inspire3D_Color_setRGB(255,0,0));

    ///// get a random number
    uint16_t random = JOY_random();
    // printf("%d\n",random);

}



int main(void){
    setup();
    while(1)
        Loop();
    return 0;
}