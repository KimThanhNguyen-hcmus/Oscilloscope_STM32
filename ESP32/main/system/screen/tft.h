#ifndef TFT_H
#define TFT_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define ORIGIN_X 120
#define ORIGIN_Y 40
#define TFT_Y_TOP 40
#define TFT_Y_BOTTOM 120
#define MAX_X 80
uint16_t COLOR_RED = 0xF800;
uint16_t COLOR_GREEN = 0x07E0;
uint16_t COLOR_BLUE = 0x001F;
uint16_t COLOR_WHITE = 0xFFFF;
uint16_t COLOR_BLACK = 0x0000;
uint16_t COLOR_YELLOW = 0xFFE0;
uint16_t COLOR_LIGHT_BLUE = 0x8410;
void TFT_SendDataBuffer(uint8_t *data, size_t len);
void TFT_HardReset(void);
void TFT_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void TFT_FillScreen(uint16_t color);
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void TFT_DrawLine(int x0, int y0, int x1, int y1, uint16_t color);
void TFT_DrawGrid();
void TFT_FillRect(int x, int y, int w, int h, uint16_t color);
void TFT_Draw_Oxy();
void TFT_DrawChar(int x, int y, char c, uint16_t color, uint16_t bg, uint8_t size);
void TFT_DrawString(int x, int y, const char *str, uint16_t color, uint16_t bg, uint8_t size);
#endif /*TFT_H*/