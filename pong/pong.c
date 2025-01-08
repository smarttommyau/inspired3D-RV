#define CH32V003_I2C_IMPLEMENTATION
#include "ch32v003fun.h"
#include <stdio.h>
#include "ch32v003_GPIO_branchless.h"

int main(void) {
    SystemInit();
    GPIO_ADCinit();
    Delay_Ms(1000);


    while(1){
        printf("ADC initialized\n");
        printf("%d %d",GPIO_analogRead(GPIO_Ain2_C4),GPIO_analogRead(GPIO_Ain3_D2));
        Delay_Ms(500);
    }
}
