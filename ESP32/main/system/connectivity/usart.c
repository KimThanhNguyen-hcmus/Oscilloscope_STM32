#include "usart.h"
#include "driver/uart.h"

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
bool USART_Recieved()
{
    uint8_t rx_byte;

    int len = uart_read_bytes(UART_NUM, &rx_byte, 1, 100 / portTICK_PERIOD_MS);
    if (len > 0 && rx_byte == 0xAA)
    {

        len = uart_read_bytes(UART_NUM, &rx_byte, 1, 10 / portTICK_PERIOD_MS);
        if (len > 0 && rx_byte == 0x55)
        {

            int data_len = uart_read_bytes(UART_NUM, data, 2048, 500 / portTICK_PERIOD_MS);
            if (data_len == 2048)
            {
                int adc_idx = 0;
                for (int k = 0; k < 2048; k += 2)
                {
                    uint8_t high_byte = data[k] & 0x0F;
                    uint8_t low_byte = data[k + 1];
                    adc_val[adc_idx] = (high_byte << 8) | low_byte;
                    adc_idx++;
                }

                printf("ADC Values: %d -> %d \n", adc_val[0], adc_val[1023]);

                return true;
            }
            else
            {
                printf("Error: Frame's length: %d\n", data_len);
            }
        }
    }

    return false;
}