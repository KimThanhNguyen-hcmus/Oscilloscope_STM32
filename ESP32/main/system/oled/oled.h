#pragma once

#include <stdint.h>
#include <stdbool.h>

void I2C_Config(void);
void OLED_WriteCmd(uint8_t cmd);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);
void OLED_DrawPixel(int x, int y, uint8_t color);
void OLED_DrawChar(int x, int y, char c);
void OLED_DrawString(int x, int y, const char *str);
void OLED_DrawGrid(void);
void OLED_DrawWaveForm(void);