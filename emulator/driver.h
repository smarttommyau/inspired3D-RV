#pragma once

#include "buttons.h"
#include "ws2812b_simple.h"
#include <stdbool.h>
#include <stdlib.h>
// Detect arrow key press
typedef enum {
    ARROW_UP    = 1,
    ARROW_RIGHT = 2,
    ARROW_DOWN  = 3,
    ARROW_LEFT  = 4,
    ARROW_NOT_FOUND = -1
} ARROW_KEY;


ARROW_KEY arrow_key_down(uint16_t adc_reading){
    if(adc_reading >= ARROW_DOWN_L && adc_reading <= ARROW_DOWN_U){
        return ARROW_DOWN;
    } else if(adc_reading >= ARROW_RIGHT_L && adc_reading <= ARROW_RIGHT_U){
        return ARROW_RIGHT;
    } else if(adc_reading >= ARROW_UP_L && adc_reading <= ARROW_UP_U){
        return ARROW_UP;
    } else if(adc_reading >= ARROW_LEFT_L && adc_reading <= ARROW_LEFT_U){
        return ARROW_LEFT;
    }
    return ARROW_NOT_FOUND;
}

typedef enum {
    ABCD_A = 1,
    ABCD_B = 2,
    ABCD_C = 3,
    ABCD_D = 4,
    ABCD_NOT_FOUND = -1
} ABCD_KEY;

ABCD_KEY abcd_key_down(uint16_t adc_reading){
    if(adc_reading >= ABCD_D_U && adc_reading <= ABCD_D_L){
        return ABCD_D;
    } else if(adc_reading >= ABCD_C_L && adc_reading <= ABCD_C_U){
        return ABCD_C;
    } else if(adc_reading >= ABCD_B_L && adc_reading <= ABCD_B_U){
        return ABCD_B;
    } else if(adc_reading >= ABCD_A_L && adc_reading <= ABCD_A_U){
        return ABCD_A;
    }
    return ABCD_NOT_FOUND;
}

#ifdef _WIN32
#define NOMINMAX 1          // Prevent Windows.h from defining min and max macros
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
void SystemInit(void) {
    // Set the console to UTF-8 mode
    SetConsoleOutputCP(65001);
    // Get the current console mode
    DWORD consoleMode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &consoleMode);
    // Enable virtual terminal processing
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), consoleMode);
}
#define Delay_Ms(milliseconds) Sleep(milliseconds)
#define Delay_Us(microseconds) Sleep((microseconds) / 1000)

static inline bool is_key_pressed(char capitalkey) {
    SHORT result =
        GetAsyncKeyState((int)capitalkey); // windows.h requires capital letters
    return (result & 0x8000) != 0;
}
uint16_t arrow_key_read_ADC(void){
    if(is_key_pressed('W')){
        return ARROW_UP_L;
    }
    if(is_key_pressed('S')){
        return ARROW_DOWN_L;
    }
    if(is_key_pressed('A')){
        return ARROW_LEFT_L;
    }
    if(is_key_pressed('D')){
        return ARROW_RIGHT_L;
    }
    return 0;//random out of range value
}

// Layout of ABCD button:
//      A
//  C   B   D
uint16_t abcd_key_read_ADC(void){
#ifdef _WIN32
    if(is_key_pressed('I')){
        return ABCD_A_L;
    }
    if(is_key_pressed('J')){
        return ABCD_C_L;
    }
    if(is_key_pressed('K')){
        return ABCD_B_L;
    }
    if(is_key_pressed('L')){
        return ABCD_D_L;
    }
    return 0;//random out of range value
}
#endif

#elif defined(__APPLE__)
#include "system_mac.h"

#include <unistd.h>

#define SystemInit() pthread_init()
#define Delay_Ms(milliseconds) usleep((milliseconds) * 1000)
#define Delay_Us(microseconds) usleep(microseconds)
uint16_t arrow_key_read_ADC(void){
    if (JOY_up_pressed()) {
        return ARROW_UP_L;
    }
    if (JOY_down_pressed()) {
        return ARROW_DOWN_L;
    }
    if (JOY_left_pressed()) {
        return ARROW_LEFT_L;
    }
    if (JOY_right_pressed()) {
        return ARROW_RIGHT_L;
    }
    // Return 0 if none are pressed
    return 0;
}

uint16_t abcd_key_read_ADC(void){
    if (JOY_top_left_pressed()) {
        return ABCD_A_L;
    }
    if (JOY_bottom_left_pressed()) {
        return ABCD_B_L;
    }
    if (JOY_top_right_pressed()) {
        return ABCD_C_L;
    }
    if (JOY_bottom_right_pressed()) {
        return ABCD_D_L;
    }
    // Return 0 if none are pressed
    return 0;
}

#endif

// #define JOY_pad_pressed()                                                                \
//     (JOY_up_pressed() || JOY_down_pressed() || JOY_left_pressed() || JOY_right_pressed()|| \
//         JOY_top_left_pressed() || JOY_top_right_pressed() || JOY_bottom_left_pressed() ||   \
//         JOY_bottom_right_pressed())
// #define JOY_pad_released()                                                               \
//     (!JOY_up_pressed() && !JOY_down_pressed() && !JOY_left_pressed() &&                  \
//         !JOY_right_pressed() && !JOY_top_left_pressed() && !JOY_top_right_pressed() &&   \
//         !JOY_bottom_left_pressed() && !JOY_bottom_right_pressed())
// #define JOY_all_released() (JOY_act_released() && !JOY_pad_releasePad())

void ADC_init(void) {
    // Do nothing
}



uint16_t rnval;
uint16_t JOY_random(void) {
    rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
    return rnval;
}

void JOY_setseed_default(void) { rnval = 0x1234; }

void JOY_setseed(uint16_t seed) { rnval = seed; }

// #define matrix_pressed_two() matrix_pressed(ADC_read)
