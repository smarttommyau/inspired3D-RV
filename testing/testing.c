// #define WS2812BSIMPLE_IMPLEMENTATION
// #include "ws2812b_simple.h"
#include "ch32v003_GPIO_branchless.h"
#define CH32V003_I2C_IMPLEMENTATION
#include "ch32v003_i2c.h"
#include <stdio.h>

#define LED_PINS GPIOA, 2


//moles coordinates
int main(void) {
    SystemInit();
    i2c_init();
    i2c_scan();
    Delay_Ms(1000);
    
    GPIO_ADCinit();

    printf("ADC initialized\n");
    while(1){
        printf("%d",GPIO_analogRead(GPIO_Ain0_A2));
    }
}
