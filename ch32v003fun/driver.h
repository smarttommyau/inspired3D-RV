#pragma once
#include "funconfig.h"
#include "ch32v003fun.h"
#include "buttons.h"
#include "ch32v003_GPIO_branchless.h"
#if FUNCONF_SYSTICK_USE_HCLK != 1
#error WS2812B Driver Requires FUNCONF_SYSTICK_USE_HCLK
#endif


// Detect arrow key press
typedef enum {
    ARROW_UP    = 1,
    ARROW_RIGHT = 2,
    ARROW_DOWN  = 3,
    ARROW_LEFT  = 4,
    ARROW_NOT_FOUND = -1
} ARROW_KEY;

// read value for detection
uint16_t arrow_key_read_ADC(void){
    // Uses Pin: 14 PC4 sw1 
    return GPIO_analogRead(GPIO_Ain2_C4);
}

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


// Layout of ABCD button:
//      A
//  C   B   D
typedef enum {
    ABCD_A = 1,
    ABCD_B = 2,
    ABCD_C = 3,
    ABCD_D = 4,
    ABCD_NOT_FOUND = -1
} ABCD_KEY;

uint16_t abcd_key_read_ADC(void){
    // Uses Pin: 19 PD2 sw2
    return GPIO_analogRead(GPIO_Ain3_D2);
}

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

uint16_t rnval;
uint16_t JOY_random(void) {
  rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
  return rnval;
}

void JOY_setseed_default(void){
  rnval = 0x1234;
}

void JOY_setseed(uint16_t seed){
  rnval = seed;
}

