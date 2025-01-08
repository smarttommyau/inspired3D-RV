#define WS2812BSIMPLE_IMPLEMENTATION
#define CH32V003_I2C_IMPLEMENTATION // using i2c
#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
#include "ws2812b_simple.h"
#include "ch32v003_i2c.h"

#define NUM_LEDS 5*5*5

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

int main(void) {
    SystemInit();
    // GPIO_ADCinit();
    i2c_init();
    i2c_scan();

    printf("I2C initialized\n");
    // int seed = 0;
    
    while(1){
        uint8_t data[NUM_LEDS][3] = {0};
        data[0][0] = 255;
        data[24][1] = 255;
        data[124][2] = 255;

        //PIN for led is 6 IN PA2
        WS2812BSimpleSend(GPIOA, PA2, (uint8_t *)data, NUM_LEDS * 3); // 3 for RGB
        printf("updated LED\n");
        Delay_Ms(1000);
    }
}
