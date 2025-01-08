#ifndef INSPIRE3D_DISPLAY
#define INSPIRE3D_DISPLAY

#include <stdint.h>
#include <stdlib.h>
#define WS2812BSIMPLE_IMPLEMENTATION
#include "ws2812b_simple.h"
#include "ch32v003_i2c.h"


typedef struct Inspire3D_Color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Inspire3D_Color;
Inspire3D_Color Inspire3D_Color_setRGB(uint8_t r, uint8_t g, uint8_t b);

void Inspire3D_Color_calculateBrightness(Inspire3D_Color * color,float brightness);

#ifdef INSPIRE3D_DISPLAY_COMMON_COLOR
#include "colors.h"
#endif

typedef struct Inspire3D_Display {
    uint8_t data[125][3];
    // 0-24 : back layer
    // ...
    // 100-124 : front layer
    // three values {g,r,b}
    GPIO_TypeDef * port; // for instance, GPIOA
    int pin; // for instance, A2
    float brightness;
} Inspire3D_Display;


Inspire3D_Display * Inspire3D_Display_Init(GPIO_TypeDef * port, int pin);

uint8_t Inspire3D_Display_Coords2Index(uint8_t x, uint8_t y, uint8_t z);

void Inspire3D_Display_Index2Coords(uint8_t index, uint8_t * x, uint8_t * y, uint8_t * z);

void Inspire3D_Display_SetColor(Inspire3D_Display * display, uint8_t index, Inspire3D_Color color);

void Inspire3D_Display_SetBGColor(Inspire3D_Display * display, Inspire3D_Color color);

void Inspire3D_Display_Update(Inspire3D_Display * display);

void Inspire3D_Display_Clear(Inspire3D_Display * display);


Inspire3D_Display * Inspire3D_Display_Init(GPIO_TypeDef * port, int pin){
    Inspire3D_Display * display = (Inspire3D_Display *)malloc(sizeof(Inspire3D_Display));
    display->port = port;
    display->pin = pin;
    display->brightness = 1.0;
    return display;
}

uint8_t Inspire3D_Display_Coords2Index(uint8_t x, uint8_t y, uint8_t z){
        return (4-y) + x*5 + z*25;
}

void Inspire3D_Display_Index2Coords(uint8_t index, uint8_t * x, uint8_t * y, uint8_t * z){
    *y = index % 5;
    *y = 4 - *y;
    *x = (index / 5) % 5;
    *z = index / 25;
}


void Inspire3D_Display_SetColor(Inspire3D_Display * display, uint8_t index, Inspire3D_Color color){
    Inspire3D_Color_calculateBrightness(&color, display->brightness);
    display->data[index][0] = color.g;
    display->data[index][1] = color.r;
    display->data[index][2] = color.b;
}
void Inspire3D_Display_SetBGColor(Inspire3D_Display * display, Inspire3D_Color color){
    Inspire3D_Color_calculateBrightness(&color, display->brightness);
    for(int i = 0; i < 125; i++){
        display->data[i][0] = color.g;
        display->data[i][1] = color.r;
        display->data[i][2] = color.b;
    }
}


void Inspire3D_Display_Update(Inspire3D_Display * display){
    WS2812BSimpleSend(display->port, display->pin, (uint8_t *)display->data, 125 * 3);
}

void Inspire3D_Display_Clear(Inspire3D_Display * display){
    Inspire3D_Display_SetBGColor(display, Inspire3D_Color_setRGB(0,0,0));
    Inspire3D_Display_Update(display);
}

Inspire3D_Color Inspire3D_Color_setRGB(uint8_t r, uint8_t g, uint8_t b){
    Inspire3D_Color color = {r, g, b};
    return color;
}

void Inspire3D_Color_calculateBrightness(Inspire3D_Color * color,float brightness){
    color->r = color->r * brightness;
    color->g = color->g * brightness;
    color->b = color->b * brightness;
}

#endif