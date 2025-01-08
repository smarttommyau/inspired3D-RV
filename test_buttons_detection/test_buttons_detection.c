#define CH32V003_I2C_IMPLEMENTATION
#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"


int main(void) {
    SystemInit();
    GPIO_ADCinit();

    Delay_Ms(1000);


    printf("ADC initialized\n");
    while(1){
        int arrow_adc   = arrow_key_read_ADC();
        int abcd_adc    = abcd_key_read_ADC();
        printf("ARROW KEY: %d\n" ,arrow_key_down(arrow_adc));
        printf("ABCD  KEY: %d\n"  ,abcd_key_down(abcd_adc));
        // check single button pressed with beloww
        // arrow_key_pressed(arrow_adc, ARROW_UP) 
        Delay_Ms(50);
    }
}
