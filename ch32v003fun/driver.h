#pragma once
#include "funconfig.h"
#include "ch32v003fun.h"
#include "buttons.h"
#include "ch32v003_GPIO_branchless.h"
#include "ch32v003_i2c.h"
#include <stdbool.h>
#if FUNCONF_SYSTICK_USE_HCLK != 1
#error WS2812B Driver Requires FUNCONF_SYSTICK_USE_HCLK
#endif


void MY_I2C_init(){
    i2c_init();
    i2c_scan();
}
#define EEPROM_ADDR 0x51 // obtained from i2c_scan()

i2c_result_e EEPROM_write(uint16_t regAddr, uint8_t *data, uint16_t sz){
    printf("EEPROM write\n");   
    return i2c_write_pages(EEPROM_ADDR, regAddr, I2C_REGADDR_2B, data, sz);
}
// for saving flash space
void void_EEPROM_write(uint16_t regAddr, uint8_t *data, uint16_t sz){
    // printf("EEPROM write\n");   
    i2c_write_pages(EEPROM_ADDR, regAddr, I2C_REGADDR_2B, data, sz);
}

i2c_result_e EEPROM_read(uint16_t regAddr, uint8_t *data, uint16_t sz){
    printf("EEPROM read\n");
    return i2c_read_pages(EEPROM_ADDR, regAddr, I2C_REGADDR_2B, data, sz);
}
// for saving flash space
void void_EEPROM_read(uint16_t regAddr, uint8_t *data, uint16_t sz){
    printf("EEPROM read\n");
    i2c_read_pages(EEPROM_ADDR, regAddr, I2C_REGADDR_2B, data, sz);
}


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

bool arrow_key_pressed(uint16_t adc_reading, ARROW_KEY detect_key){
    return (arrow_key_down(adc_reading) == detect_key);
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


// Turn i2c mode on when using with inspire3d_display
// And when the value is wrong
uint16_t abcd_key_read_ADC(void){
    // Uses Pin: 19 PD2 sw2
    return GPIO_analogRead(GPIO_Ain3_D2);
}

uint16_t abcd_key_read_ADC_D3(void){
    // Uses Pin: D3 , also contains info
    return GPIO_analogRead(GPIO_Ain4_D3);
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

bool abcd_key_pressed(uint16_t adc_reading, ABCD_KEY detect_key) {
    return (abcd_key_down(adc_reading) == detect_key);
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

