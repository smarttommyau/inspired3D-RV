#pragma once

#include "buttons.h"
#include "ws2812b_simple.h"
#include <stdbool.h>
#include <stdlib.h>


void MY_I2C_init(){
    WS_init();
}

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

#define arrow_up_pressed() is_key_pressed('W')  
#define arrow_down_pressed() is_key_pressed('S')
#define arrow_left_pressed() is_key_pressed('A')
#define arrow_right_pressed() is_key_pressed('D')

#define abcd_a_pressed() is_key_pressed('I')
#define abcd_b_pressed() is_key_pressed('K')
#define abcd_c_pressed() is_key_pressed('J')
#define abcd_d_pressed() is_key_pressed('L')

#elif defined(__APPLE__)
#include "system_mac.h"

#include <unistd.h>

#define SystemInit() pthread_init()
#define Delay_Ms(milliseconds) usleep((milliseconds) * 1000)
#define Delay_Us(microseconds) usleep(microseconds)

#define arrow_up_pressed() is_key_pressed(W_key)
#define arrow_down_pressed() is_key_pressed(S_key)
#define arrow_left_pressed() is_key_pressed(A_key)
#define arrow_right_pressed() is_key_pressed(D_key)

#define abcd_a_pressed() is_key_pressed(I_key)
#define abcd_b_pressed() is_key_pressed(K_key)
#define abcd_c_pressed() is_key_pressed(J_key)
#define abcd_d_pressed() is_key_pressed(L_key)

#endif


uint16_t arrow_key_read_ADC(void){
    if(adc_arrow > 0){
        return adc_arrow;
    }
    if(arrow_up_pressed()){
        return ARROW_UP_L;
    }
    if(arrow_down_pressed()){
        return ARROW_DOWN_L;
    }
    if(arrow_left_pressed()){
        return ARROW_LEFT_L;
    }
    if(arrow_right_pressed()){
        return ARROW_RIGHT_L;
    }
    return 0; // random out of range value
}

// Layout of ABCD button:
//      A
//  C   B   D
uint16_t abcd_key_read_ADC(void){
    if(adc_abcd > 0){
        return adc_abcd;
    }
    if(abcd_a_pressed()){
        return ABCD_A_L;
    }
    if(abcd_c_pressed()){
        return ABCD_C_L;
    }
    if(abcd_b_pressed()){
        return ABCD_B_L;
    }
    if(abcd_d_pressed()){
        return ABCD_D_L;
    }
    return 0; // random out of range value
}

bool arrow_key_pressed(uint16_t adc_reading, ARROW_KEY detect_key){
    return (arrow_key_down(adc_reading) == detect_key);
}   
bool abcd_key_pressed(uint16_t adc_reading, ABCD_KEY detect_key) {
    return (abcd_key_down(adc_reading) == detect_key);
}
void ADC_init(void) {
    // Do nothing
}

void GPIO_ADCinit(){
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
