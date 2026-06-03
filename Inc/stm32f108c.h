#ifndef STM32F103C
#define STM32F103C
#include <stdint.h>
/*BASE ADDRESS */
#define PERIPH_BASE 0x40000000UL
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000UL)
#define APB1PERIPH_BASE (PERIPH_BASE)
#define AHBPERIPH_BASE (PERIPH_BASE + 0x20000UL)
/*RCC's ADDRESS*/
#define RCC_PERIPH_BASE (AHBPERIPH_BASE + 0x1000UL)
/*GPIO's ADDRESS*/
#define GPIOC_PERIPH_BASE (APB2PERIPH_BASE + 0x1000UL)
#define GPIOB_PERIPH_BASE (APB2PERIPH_BASE + 0x0C00UL)
#define GPIOA_PERIPH_BASE (APB2PERIPH_BASE + 0x0800UL)
/*DMA's ADDRESS*/
#define DMA1_PERIPH_BASE AHBPERIPH_BASE
#define DMA2_PERIPH_BASE (AHBPERIPH_BASE + 0x0800UL)
/*ADC's ADDRESS*/
#define ADC1_PERIPH_BASE (APB2PERIPH_BASE + 0x2400UL)
#define ADC2_PERIPH_BASE (APB2PERIPH_BASE + 0x2800UL)
#define ADC3_PERIPH_BASE (APB2PERIPH_BASE + 0x3C00UL)
/*I2C's ADDRESS*/
#define I2C1_PERIPH_BASE (APB1PERIPH_BASE + 0x5400UL)
/*USART's ADDRESS*/
#define USART1_PERIPH_BASE (APB2PERIPH_BASE + 0x3800UL)
/*TIMER's ADDRESS*/
#define TIMER1_PERIPH_BASE (APB2PERIPH_BASE + 0x2C00UL)
#define TIMER2_PERIPH_BASE (APB1PERIPH_BASE + 0x0000UL)
#define TIMER3_PERIPH_BASE (APB1PERIPH_BASE + 0x0400UL)
/*SYSTICK's ADDRESS*/
#define SYSTICK_BASE 0xE000E010
/*FLASH's ADDDRESS*/
#define FLASH_BASE (AHBPERIPH_BASE + 0x2000UL)
typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t AHBRSTR;
    volatile uint32_t CFGR2;
} RCC_TypeDef;

typedef struct
{
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
    volatile uint32_t CCR1;
    volatile uint32_t CNDTR1;
    volatile uint32_t CPAR1;
    volatile uint32_t CMAR1;
    volatile uint32_t CCR2;
    volatile uint32_t CNDTR2;
    volatile uint32_t CPAR2;
    volatile uint32_t CMAR2;
    volatile uint32_t CCR3;
    volatile uint32_t CNDTR3;
    volatile uint32_t CPAR3;
    volatile uint32_t CMAR3;
    volatile uint32_t CCR4;
    volatile uint32_t CNDTR4;
    volatile uint32_t CPAR4;
    volatile uint32_t CMAR4;

} DMA_TypeDef;

typedef struct
{
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
} I2C_TypeDef;

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR1;
    volatile uint32_t JOFR2;
    volatile uint32_t JOFR3;
    volatile uint32_t JOFR4;
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t JSOR;
    volatile uint32_t JDR1;
    volatile uint32_t JDR2;
    volatile uint32_t JDR3;
    volatile uint32_t JDR4;
    volatile uint32_t DR;
} ADC_TypdeDef;

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    uint32_t re0;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    uint32_t re1;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIMER_TypeDef;

typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} Systick_TypeDef;

typedef struct
{
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    uint32_t re1;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} Flash_TypeDef;

#define RCC ((RCC_TypeDef *)RCC_PERIPH_BASE)

#define DMA1 ((DMA_TypeDef *)DMA1_PERIPH_BASE)
#define DMA2 ((DMA_TypeDef *)DMA2_PERIPH_BASE)

#define GPIOA ((GPIO_TypeDef *)GPIOA_PERIPH_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_PERIPH_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_PERIPH_BASE)

#define USART1 ((USART_TypeDef *)USART1_PERIPH_BASE)

#define I2C1 ((I2C_TypeDef *)I2C1_PERIPH_BASE)

#define ADC1 ((ADC_TypdeDef *)ADC1_PERIPH_BASE)

#define TIM1 ((TIMER_TypeDef *)TIMER1_PERIPH_BASE)
#define TIM2 ((TIMER_TypeDef *)TIMER2_PERIPH_BASE)
#define TIM3 ((TIMER_TypeDef *)TIMER3_PERIPH_BASE)

#define SYSTICK ((Systick_TypeDef *)SYSTICK_BASE)

#define FLASH ((Flash_TypeDef *)FLASH_BASE)
#endif