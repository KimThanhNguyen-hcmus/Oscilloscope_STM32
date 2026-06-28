#include "usart.h"

#define UART_NUM UART_NUM_1
#define MAX_PAYLOAD 2048
#define BUF_SIZE (2048 * 2)

typedef enum
{
    STATE_IDLE_1,
    STATE_IDLE_2,
    STATE_GET_LENGTH_H,
    STATE_GET_LENGTH_L,
    STATE_GET_LENGTH,
    STATE_GET_PAYLOAD,
    STATE_GET_CHECKSUM
} USART_State_t;

static uint8_t rx_buffer[MAX_PAYLOAD];
static usart_callback_t g_usart_cb = NULL;

void USART_Register_Callback(usart_callback_t cb)
{
    g_usart_cb = cb;
}

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
void USART_Received()
{
    static USART_State_t current_state = STATE_IDLE_1;
    static uint16_t packetlen = 0;
    static uint16_t rx_index = 0;
    static uint8_t calc_checksum = 0;

    uint8_t temp_buf[256];
    int len = uart_read_bytes(UART_NUM, temp_buf, sizeof(temp_buf), pdMS_TO_TICKS(10));

    if (len > 0)
    {
        for (int i = 0; i < len; i++)
        {
            uint8_t rx_byte = temp_buf[i];
            switch (current_state)
            {
            case STATE_IDLE_1:
                if (rx_byte == 0xAA)
                    current_state = STATE_IDLE_2;
                break;

            case STATE_IDLE_2:
                if (rx_byte == 0x55)
                    current_state = STATE_GET_LENGTH_H;
                else
                    current_state = STATE_IDLE_1;
                break;

            case STATE_GET_LENGTH_H:
                packetlen = (uint16_t)(rx_byte << 8);
                calc_checksum = rx_byte;
                current_state = STATE_GET_LENGTH_L;
                break;

            case STATE_GET_LENGTH_L:
                packetlen |= rx_byte;
                calc_checksum ^= rx_byte;
                rx_index = 0;

                if (packetlen > 0 && packetlen <= MAX_PAYLOAD)
                {
                    current_state = STATE_GET_PAYLOAD;
                }
                else
                {
                    current_state = STATE_IDLE_1;
                }
                break;

            case STATE_GET_PAYLOAD:
                rx_buffer[rx_index++] = rx_byte;
                calc_checksum ^= rx_byte;

                if (rx_index >= packetlen)
                {
                    current_state = STATE_GET_CHECKSUM;
                }
                break;

            case STATE_GET_CHECKSUM:
                if (rx_byte == calc_checksum)
                {
                    if (g_usart_cb != NULL)
                    {
                        g_usart_cb(rx_buffer, packetlen);
                    }
                }
                else
                {
                    printf("Loi Checksum! ESP32 tinh ra: 0x%02X, STM32 gui toi: 0x%02X\n", calc_checksum, rx_byte);
                    uart_flush_input(UART_NUM);
                }
                current_state = STATE_IDLE_1;
                break;

            default:
                current_state = STATE_IDLE_1;
                break;
            }
        }
    }
}