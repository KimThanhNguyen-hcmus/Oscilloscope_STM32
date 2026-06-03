#include "systemclk.h"

void SystemClock_Config(void)
{
    RCC->CR |= (1 << 16); /*Enable HSE clock*/
    while (!(RCC->CR & (1 << 17)))
        ; /*Wait HSE Flag ready*/
    /*------FLASH LATENCY------*/
    FLASH->ACR |= (1 << 4);    /*Enable Prefetch buffer*/
    FLASH->ACR &= ~(0x7 << 0); /*Clear register*/
    FLASH->ACR |= (0x2 << 0);  /*0x010 mean 48MHz < SYSTICK < 72MHz*/
    /*--------BUS CONFIG--------*/
    RCC->CFGR &= ~(0xF << 4);  /*AHP Bus's prescaler not divided -> HCLK = 72MHz*/
    RCC->CFGR &= ~(0x7 << 8);  /*Clear register*/
    RCC->CFGR |= (0x4 << 8);   /*APB1 Bus's prescaler divided by 2 -> PCLK1 = 36MHz*/
    RCC->CFGR &= ~(0x7 << 11); /*APB2 Bus's prescaler not divided -> PCLK2 = 72MHz*/
    RCC->CFGR |= (1 << 16);    /*Enable PLL entry clock source -> HSE as PLL input clock*/
    RCC->CFGR &= ~(0xF << 18); /*Clear register*/
    RCC->CFGR |= (0x7 << 18);  /*PLL multiplication factor -> PLL input clock x 9*/
    RCC->CR |= (1 << 24);      /*Enabel PLL on*/
    while (!(RCC->CR & (1 << 25)))
        ;                     /*Wait PLL Flag ready*/
    RCC->CFGR &= ~(0x3 << 0); /*Clear register*/
    RCC->CFGR |= (0x2 << 0);  /*PLL selected as system clock*/
    while ((RCC->CFGR & (0x3 << 2)) != (0x2 << 2))
        ; /*Wait and check for PLL is used as systemclock*/
}