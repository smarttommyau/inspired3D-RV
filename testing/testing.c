#define CH32V003_I2C_IMPLEMENTATION
#include "ch32v003fun.h"
#include <stdio.h>
#include "ch32v003_GPIO_branchless.h"
#include "ch32v003_i2c.h"

// #define LED_PINS GPIOA, 2


//NOTE:
//PA1: GPIO_Ain1_A1
//PA2: GPIO_Ain0_A2
//PC4: GPIO_Ain2_C4 sw1
//PD2: GPIO_Ain3_D2 sw2
//PD3: GPIO_Ain4_D3
//PD4: GPIO_Ain7_D4
//PD5: GPIO_Ain5_D5
//PD6: GPIO_Ain6_D6

// Arrow Key values:
// ~32? : no key pressed
// ~160 : down
// ~328 : right
// ~220 : down + right
// ~496 : up 
// ~685 : left


// Note: it seems that pressing two key at once does not give good output so ignore those case
// ABCD key values:(D2,D3)
// ~32?  ~200: no key pressed
// ~160  ~300: D
// ~220  ~330: C+D
// <=326 340-380: C
// ~430-500 ~420-470: B+C
// ~496 ~480: B
// ~335 >300: B+D
// ~680 : A
// ~680 ~500 : A+Any other key

//moles coordinates
int main(void) {
    SystemInit();
    GPIO_ADCinit();
    i2c_init();
    i2c_scan();
    Delay_Ms(1000);


    while(1){
        printf("ADC initialized\n");
        printf("%d %d",GPIO_analogRead(GPIO_Ain2_C4),GPIO_analogRead(GPIO_Ain3_D2));
        Delay_Ms(500);
    }
}
