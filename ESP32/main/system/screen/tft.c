#include "tft.h"
#include "spi.h"

uint16_t old_screen_y[MAX_X];
int current_logic_x = 0;
int prev_screen_y = ORIGIN_Y;

long map_val(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void TFT_SendDataBuffer(uint8_t *data, size_t len)
{
    if (len == 0)
        return;
    gpio_set_level(PIN_NUM_DC, 1);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}
void TFT_HardReset(void)
{
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(150 / portTICK_PERIOD_MS);
}
void TFT_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    TFT_SendCommand(0x2A); // CASET - Column Address Set
    TFT_SendData(x0 >> 8);
    TFT_SendData(x0 & 0xFF);
    TFT_SendData(x1 >> 8);
    TFT_SendData(x1 & 0xFF);

    TFT_SendCommand(0x2B); // RASET - Row Address Set
    TFT_SendData(y0 >> 8);
    TFT_SendData(y0 & 0xFF);
    TFT_SendData(y1 >> 8);
    TFT_SendData(y1 & 0xFF);

    TFT_SendCommand(0x2C); // RAMWR - Memory Write
}
void TFT_FillScreen(uint16_t color)
{
    TFT_SetWindow(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    uint8_t color_hi = color >> 8;
    uint8_t color_lo = color & 0xFF;
    uint8_t line_buffer[TFT_WIDTH * 2];
    for (int i = 0; i < TFT_WIDTH * 2; i += 2)
    {
        line_buffer[i] = color_hi;
        line_buffer[i + 1] = color_lo;
    }
    for (int y = 0; y < TFT_HEIGHT; y++)
    {
        TFT_SendDataBuffer(line_buffer, sizeof(line_buffer));
    }
}
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT)
        return;
    TFT_SetWindow(x, y, x, y);
    uint8_t data[2] = {color >> 8, color & 0x0FF};
    TFT_SendDataBuffer(data, 2);
}
void TFT_FillRect(int x, int y, int w, int h, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT || w <= 0 || h <= 0)
        return;
    if (x + w > TFT_WIDTH)
        w = TFT_WIDTH - x;
    if (y + h > TFT_HEIGHT)
        h = TFT_HEIGHT - y;
    TFT_SetWindow(x, y, x + w - 1, y + h - 1);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < w * h; i++)
    {
        TFT_SendDataBuffer(data, 2);
    }
}
void TFT_DrawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1)
    {
        TFT_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
void TFT_DrawFastVLine(int x, int y, int h, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT || h <= 0)
        return;
    if (y + h - 1 >= TFT_HEIGHT)
        h = TFT_HEIGHT - y;
    TFT_SetWindow(x, y, x, y + h - 1);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < h; i++)
    {
        TFT_SendDataBuffer(data, 2);
    }
}

void TFT_DrawFastHLine(int x, int y, int w, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT || w <= 0)
        return;
    if (x + w - 1 >= TFT_WIDTH)
        w = TFT_WIDTH - x;

    TFT_SetWindow(x, y, x + w - 1, y);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < w; i++)
    {
        TFT_SendDataBuffer(data, 2);
    }
}
void TFT_DrawGrid()
{
    uint16_t COLOR_GRID = COLOR_YELLOW;
    for (int x = 0; x < TFT_WIDTH; x += 40)
    {
        TFT_DrawFastVLine(x, 0, TFT_HEIGHT, COLOR_GRID);
    }
    for (int y = 0; y < TFT_HEIGHT; y += 40)
    {
        TFT_DrawFastHLine(0, y, TFT_WIDTH, COLOR_GRID);
    }
}

void TFT_Draw_Oxy()
{
    for (int i = 0; i < 3; i++)
    {
        TFT_DrawLine(40, 120 + i, 320, 120 + i, COLOR_YELLOW);
    }
    for (int i = 0; i < 3; i++)
    {
        TFT_DrawLine(40 + i, 40, 40 + i, 200, COLOR_YELLOW);
    }
}
