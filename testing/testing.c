// #define WS2812BSIMPLE_IMPLEMENTATION
// #include "ws2812b_simple.h"
#include "ch32v003_GPIO_branchless.h"
#include "ch32v003_i2c.h"
#include <stdio.h>

#define LED_PINS GPIOA, 2


//moles coordinates
int main(void) {
    SystemInit();
    i2c_init();
    i2c_scan();
    GPIO_ADCinit();
    // Enable GPIOD.
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;

	// GPIO D0 Push-Pull, 10MHz Output
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

    printf("ADC initialized\n");
    while(1){
        printf("%d",GPIO_analogRead(GPIO_Ain0_A2));
    }
}
