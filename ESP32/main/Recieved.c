#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define UART_NUM UART_NUM_2
#define BUF_SIZE (1024 * 2)
#define DATA_LENGTH 1024

uint8_t data[2050];
uint8_t oled_buffer[1024];

i2c_master_dev_handle_t oled_handle;

void USART_Config()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    QueueHandle_t uart_queue;
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 10, &uart_queue, 0));
}
void USART_Recieved()
{
    int length = uart_read_bytes(UART_NUM, data, sizeof(data), 500 / portTICK_PERIOD_MS);
    if (length > 0)
    {
        for (int i = 0; i < length - 1; i++)
        {
            if (data[i] == 0xAA && data[i + 1] == 0x55)
            {
                int adc_idx = 0;
                for (int j = i + 2; j < length - 1 && adc_idx < 1024; j += 2)
                {
                    uint8_t high_byte = data[j];
                    uint8_t low_byte = data[j + 1];
                    adc_val[adc_idx] = (high_byte << 8) | low_byte;
                    adc_idx++;
                }
                printf("ADC Values: %d -> %d \n", adc_val[0], adc_val[1023]);
                break;
            }
        }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
void I2C_Config()
{
    /*-------Master-------*/
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,
        .scl_io_num = 22,
        .sda_io_num = 21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x3C,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &oled_handle));
}
void OLED_WrireCmd(uint8_t cmd)
{
    uint8_t buffer[2] = {0x00, cmd};
    i2c_master_transmit(oled_handle, buffer, 2, -1);
}
void OLED_Init()
{
    uint8_t init_cmds[] = {
        0xAE,
        0x20, 0x00,
        0xC8,
        0xA1,
        0x81, 0xFF,
        0xAF};
    for (int i = 0; i < sizeof(init_cmds); i++)
    {
        OLED_WrireCmd(init_cmds[i]);
    }
}
void OLED_Clear()
{
    memset(oled_buffer, 0, sizeof(oled_buffer));
}
void OLED_Update()
{
    OLED_WriteCmd(0x21);
    OLED_WriteCmd(0);
    OLED_WriteCmd(127);
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(0);
    OLED_WriteCmd(7);
    uint8_t send_buf[1025];
    send_buf[0] = 0x40;
    memcpy(&send_buf[1], oled_buffer, 1024);
    i2c_master_transmit(oled_handle, send_buf, 1025, -1);
}
void OLED_DrawWaveForm()
{
    OLED_Clear();
    for (int x = 0; x < 128; x++)
    {
        int y = 63 - ((uint32_t)adc_val[x] * 63 / 4095);

        if (y < 0)
            y = 0;
        if (y > 63)
            y = 63;
        oled_buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    }
    OLED_Update();
}
void app_main(void)
{
    while (1)
    {
    }
}
