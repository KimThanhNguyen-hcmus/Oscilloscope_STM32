#include "stm32f108c.h"
#include "systemclk.h"
#include "delay.h"
uint16_t adc_buffer[1024];

void ADC_Config()
{
    RCC->APB2ENR |= (1 << 9) | (1 << 2);
    GPIOA->CRL &= ~(0xF << 4);
    GPIOA->CRL |= (0x0 << 4);
    ADC1->CR2 = 0;
    ADC1->SQR3 &= ~(0xF << 0);
    ADC1->SQR3 |= 1;
    ADC1->SMPR2 |= (6 << 3);
}

void DMA_Config(uint32_t pg_address, uint32_t mb_address, uint32_t size)
{
    /*---------DMA----------*/
    RCC->AHBENR |= (1 << 0); /*Enable Clock*/
    DMA1->CCR1 = 0;          /*Clear Register*/
    /*Enable Circular Mode, Memory increment mode, Peripheral and Memory Size*/
    DMA1->CCR1 |= (1 << 5) | (1 << 7) | (1 << 8) | (1 << 10);
    DMA1->CPAR1 = pg_address; /*Peripheral's Address*/
    DMA1->CMAR1 = mb_address; /*Memory's Address*/
    DMA1->CNDTR1 = size;      /*Number of data Register*/
    DMA1->CCR1 |= (1 << 0);   /*Channel enable*/
    /*---------ADC----------*/
    ADC1->CR2 |= (1 << 0); /*Enable A/D converter*/
    for (volatile int i = 0; i < 10000; i++)
        ;                  /*Wait from stablize*/
    ADC1->CR2 |= (1 << 2); /*Enable Calibration*/
    while (ADC1->CR2 & (1 << 2))
        ; /*Wait for Calibration complete*/
    // ADC1->CR2 |= (1 << 1); /*Continuous conversion */
    ADC1->CR2 |= (1 << 8); /*DMA mode enable*/

    ADC1->CR2 |= (1 << 20);    /*Conversion on external event enable*/
    ADC1->CR2 &= ~(0x7 << 17); /*Clear bits*/
    ADC1->CR2 |= (4 << 17);    /*Select external event for regular group - Enable Timer3 TRGO event*/

    ADC1->CR2 |= (1 << 0); /*Enable A/D converter*/
}

void TIMER_Config()
{
    /*Enable Clock*/
    RCC->APB1ENR |= (1 << 1);
    /*Sample at 100kHz -> f = 70MHz / ((PSC + 1) * (ARR + 1))*/
    TIM3->PSC = 71;
    TIM3->ARR = 9;

    TIM3->CR2 &= ~(0x7 << 4);
    TIM3->CR2 |= (0x2 << 4);

    TIM3->CR1 |= (1 << 0);
}

void USART_Config()
{
    RCC->APB2ENR |= (1 << 14) | (1 << 2);
    GPIOA->CRH &= ~((0xF << 4) | (0xF << 8));
    GPIOA->CRH |= (0xB << 4) | (0x8 << 8);
    USART1->CR1 |= (1 << 13) | (1 << 3) | (1 << 2);
    USART1->BRR = 625; /*Baudrate = 115200*/
}
void USART_Send(char ch)
{
    while (!(USART1->SR & (1 << 7)))
    {
    }
    USART1->DR = (0xFF & ch);
}
char USART_Recieve()
{
    while (!(USART1->SR & (1 << 5)))
    {
    }
    return (char)(USART1->DR & 0xFF);
}
void I2C_Config()
{
    RCC->APB2ENR |= (1 << 3);
    RCC->APB1ENR |= (1 << 21);
    GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
    GPIOB->CRL |= (0xF << 24) | (0xF << 28);
    I2C1->CR1 |= (1 << 15);
    I2C1->CR1 &= ~(1 << 15);
    I2C1->CR2 = 36;
    I2C1->CCR = 180;  // CCR = f_APB1 / (2 * 100kHz) = 36MHz / (2 * 100kHz) = 40
    I2C1->TRISE = 37; // TRISE = (1000ns * f_APB1) + 1 = 37 + 1 = 9
    I2C1->CR1 |= (1 << 0);
}
void I2C_SendData(uint8_t slave_addr, uint8_t controlbyte, uint8_t data)
{
    I2C1->CR1 |= (1 << 8);
    while (!(I2C1->SR1 & (1 << 0)))
        ;
    I2C1->DR = (slave_addr << 1);
    while (!(I2C1->SR1 & (1 << 1)))
        ;
    (void)I2C1->SR2;
    while (!(I2C1->SR1 & (1 << 7)))
        ;
    I2C1->DR = controlbyte;
    while (!(I2C1->SR1 & (1 << 7)))
        ;
    I2C1->DR = data;
    while (!(I2C1->SR1 & (1 << 2)))
        ;
    I2C1->CR1 |= (1 << 9);
}
void OLED_Wakeup(void)
{
    delay_ms(100);
    uint8_t inits_cmd[] = {0xAE, 0x20, 0x00, 0x8D, 0x14, 0xAF, 0xA4};
    for (int i = 0; i < sizeof(inits_cmd); i++)
    {
        I2C_SendData(0x3C, 0x00, inits_cmd[i]);
    }
}
uint32_t Calculate_ADC()
{
    uint32_t adc_raw = ADC1->DR;
    return adc_raw * 3.3 / 4096;
}
uint8_t oled_test_data[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int main(void)
{
    SystemClock_Config();
    ADC_Config();
    DMA_Config((uint32_t)&ADC1->DR, (uint32_t)adc_buffer, 1024);
    // DMA_Config(Calculate_ADC(), (uint32_t)adc_buffer, 1024);
    TIMER_Config();
    USART_Config();
    while (1)
    {
        USART_Send(0xAA);
        USART_Send(0x55);
        for (uint16_t i = 0; i < sizeof(adc_buffer) / sizeof(adc_buffer[0]); i++)
        {
            uint8_t high_byte = (uint8_t)((adc_buffer[i] >> 8) & 0xFF);
            uint8_t low_byte = (uint8_t)(adc_buffer[i] & 0xFF);
            USART_Send(high_byte);
            USART_Send(low_byte);
        }
        delay_ms(50);
    }
}