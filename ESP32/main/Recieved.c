#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "system/connectivity/spi.c"
#include "system/connectivity/usart.c"
#include "system/screen/tft.c"

#define FILTER_ORDER 8
#define MAX_SAMPLES_PER_PACKET 256

static const char *TAG = "TFT_TEST";
const float b[FILTER_ORDER] = {0.013671, 0.070971, 0.159492, 0.255866, 0.255866, 0.159492, 0.070971, 0.013671};
float x_history[FILTER_ORDER] = {0};
uint16_t adc_val_clean[1024];
uint16_t length_adc = sizeof(adc_val_clean) / sizeof(adc_val_clean[0]);
volatile bool new_data_ready = false;
volatile int current_adc_length = 0;
QueueHandle_t graph_queue = NULL;
typedef struct
{
    uint16_t data[MAX_SAMPLES_PER_PACKET];
    int length;
} GraphData_t;
GraphData_t packet;

float fir_filter(float adc_val)
{
    float y = 0.0;
    for (int i = FILTER_ORDER - 1; i > 0; i--)
    {
        x_history[i] = x_history[i - 1];
    }
    x_history[0] = adc_val;
    for (int i = 0; i < FILTER_ORDER; i++)
    {
        y += b[i] * x_history[i];
    }
    return y;
}
void On_UART_Packet_Received(const uint8_t *payload, int length)
{

    packet.length = 0;
    for (int k = 0; k < length; k += 2)
    {
        if (packet.length < MAX_SAMPLES_PER_PACKET)
        {
            uint16_t raw_adc = (payload[k] << 8) | payload[k + 1];
            packet.data[packet.length++] = raw_adc;
        }
    }
    printf("ADC Values: %d -> %d!\n", packet.data[0], packet.data[packet.length - 1]);
    if (graph_queue != NULL)
    {
        xQueueSend(graph_queue, &packet, 0);
    }
}
void Task_UART_Receive(void *pvParameters)
{
    while (1)
    {
        USART_Received();
    }
}
void Task_Draw_TFT(void *pvParameters)
{
    int START_X = 40;
    int END_X = 280;
    int current_x = START_X;
    int prev_x = START_X;
    int prev_y = 40;

    while (1)
    {
        if (xQueueReceive(graph_queue, &packet, portMAX_DELAY) == pdTRUE)
        {
            for (int i = 0; i < packet.length; i++)
            {
                int pixel_y = map_val(packet.data[i], 0, 4095, TFT_Y_BOTTOM, TFT_Y_TOP);
                if (pixel_y < TFT_Y_TOP)
                    pixel_y = TFT_Y_TOP;
                if (pixel_y > 120)
                    pixel_y = 120;
                int clear_x = current_x + 2;
                if (clear_x < END_X)
                {

                    TFT_FillRect(clear_x, TFT_Y_TOP, 4, TFT_Y_BOTTOM - TFT_Y_TOP + 1, COLOR_BLACK);
                    for (int i = 0; i < 4; i++)
                    {
                        TFT_DrawPixel(clear_x + i, 119, COLOR_YELLOW);
                        TFT_DrawPixel(clear_x + i, 120, COLOR_YELLOW);
                        TFT_DrawPixel(clear_x + i, 121, COLOR_YELLOW);
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        int check_x = clear_x + i;
                        if (check_x == 159 || check_x == 160 || check_x == 161)
                        {
                            TFT_DrawFastVLine(check_x, TFT_Y_TOP, TFT_Y_BOTTOM - TFT_Y_TOP + 1, COLOR_YELLOW);
                        }
                    }
                    uint16_t COLOR_GRID = COLOR_WHITE;
                    if ((clear_x % 4) < 2)
                    {
                        for (int grid_y = TFT_Y_TOP; grid_y <= TFT_Y_BOTTOM; grid_y += 40)
                        {
                            TFT_DrawPixel(clear_x, grid_y, COLOR_GRID);
                        }
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        if ((clear_x + i) % 40 == 0)
                        {
                            for (int grid_y = TFT_Y_TOP; grid_y <= TFT_Y_BOTTOM; grid_y += 4)
                            {
                                TFT_DrawPixel(clear_x + i, grid_y, COLOR_GRID);
                                TFT_DrawPixel(clear_x + i, grid_y + 1, COLOR_GRID);
                            }
                        }
                    }
                }
                if (current_x <= START_X)
                {
                    TFT_DrawPixel(current_x, pixel_y, COLOR_GREEN);
                }
                else
                {

                    int y_min = (prev_y < pixel_y) ? prev_y : pixel_y;
                    int y_max = (prev_y > pixel_y) ? prev_y : pixel_y;
                    TFT_DrawFastVLine(current_x, y_min, y_max - y_min + 1, COLOR_GREEN);
                }

                prev_x = current_x;
                prev_y = pixel_y;
                current_x++;
                if (current_x > END_X)
                {
                    current_x = START_X;
                }
            }
        }
    }
}
void app_main(void)
{
    ESP_LOGI("DEBUG", "Bat dau khoi dong...");

    GPIO_Config();
    SPI_Config();
    USART_Config();

    ESP_LOGI("DEBUG", "Reset TFT...");
    TFT_HardReset();

    ESP_LOGI("DEBUG", "Gui lenh Init...");
    TFT_SendCommand(0x01);
    vTaskDelay(150 / portTICK_PERIOD_MS);

    TFT_SendCommand(0x11); // Sleep Out
    vTaskDelay(500 / portTICK_PERIOD_MS);

    TFT_SendCommand(0x3A); // Color Mode
    TFT_SendData(0x55);    // 16-bit RGB565

    TFT_SendCommand(0x36); // Memory Access Control
    TFT_SendData(0xA0);

    TFT_SendCommand(0x29); // Display ON
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Filling BLACK");
    TFT_FillScreen(COLOR_BLACK);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    TFT_DrawGrid();
    TFT_Draw_Oxy();

    graph_queue = xQueueCreate(10, sizeof(GraphData_t));
    USART_Register_Callback(On_UART_Packet_Received);

    xTaskCreatePinnedToCore(Task_UART_Receive, "UART_Rx", 4096, NULL, 5, NULL, 0);

    xTaskCreatePinnedToCore(Task_Draw_TFT, "TFT_Draw", 8192, NULL, 4, NULL, 1);

    ESP_LOGI("DEBUG", "OS Started! Cho data do ve...");
}