#pragma once
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "tft.h"

#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5
#define PIN_NUM_DC 27
#define PIN_NUM_RST 4

extern spi_device_handle_t spi;

void SPI_Config(void);
void GPIO_Config(void);
void TFT_SendData(uint8_t data);
void TFT_SendCommand(uint8_t cmd);