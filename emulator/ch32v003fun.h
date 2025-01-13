#pragma once
#include <stdint.h>

#define GPIOA ((GPIO_TypeDef *)0)
#define GPIOC ((GPIO_TypeDef *)0)
#define GPIOD ((GPIO_TypeDef *)0)
#define __IO volatile

#define PA1 1
#define PA2 2
#define PC0 32
#define PC1 33
#define PC2 34
#define PC3 35
#define PC4 36
#define PC5 37
#define PC6 38
#define PC7 39
#define PD0 48
#define PD1 49
#define PD2 50
#define PD3 51
#define PD4 52
#define PD5 53
#define PD6 54
#define PD7 55

typedef struct {
    __IO uint32_t CFGLR;
    __IO uint32_t CFGHR;
    __IO uint32_t INDR;
    __IO uint32_t OUTDR;
    __IO uint32_t BSHR;
    __IO uint32_t BCR;
    __IO uint32_t LCKR;
} GPIO_TypeDef;

#define NVIC_SystemReset() // suppose to restart the program, unimplemented
