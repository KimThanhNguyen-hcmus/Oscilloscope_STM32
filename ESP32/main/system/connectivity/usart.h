#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver/uart.h"

typedef void (*usart_callback_t)(const uint8_t *payload, int length);
void USART_Config(void);
void USART_Register_Callback(usart_callback_t cb);
void USART_Received(void);

#endif