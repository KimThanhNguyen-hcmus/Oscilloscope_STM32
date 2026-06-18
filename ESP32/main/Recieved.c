#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#define UART_NUM UART_NUM_2
#define BUF_SIZE (1024 * 2)
#define DATA_LENGTH 1024
#define N 16
#define WIFI_SSID "Nha Tret 2.4G"
#define PASSWORD "01684306403"
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define TAG "WEBSERVER"
uint8_t data[2050];
uint8_t oled_buffer[1024];
uint16_t adc_val[1024];
i2c_master_dev_handle_t oled_handle;

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

float b[N] = {
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625};

const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space (0x20)
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 (0x30)
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A (0x41)
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
};
float x_buffer[N] = {0};
int buffer_index = 0;
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
static httpd_handle_t global_server = NULL;
static int current_ws_fd = 1;

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

float Firfilter(float input)
{
    float output = 0;
    x_buffer[buffer_index] = input;
    for (int i = 0; i < N; i++)
    {
        int tap_index = (buffer_index - i + N) % N;
        output += b[i] * x_buffer[tap_index];
    }
    buffer_index = (buffer_index + 1) % N;
    return output;
}

void OLED_WriteCmd(uint8_t cmd)
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
        OLED_WriteCmd(init_cmds[i]);
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
void OLED_DrawPixel(int x, int y, uint8_t color)
{
    if (x < 0 || x >= 128 || y < 0 || y > 63)
        return;
    if (color)
    {
        oled_buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    }
    else
    {
        oled_buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
    }
}
void OLED_DrawChar(int x, int y, char c)
{
    if (c < 0x20 || c > 0x5A)
        return;
    int font_idx = c - 0x20;
    for (int col = 0; col < 5; col++)
    {
        uint8_t line = font5x7[font_idx][col];
        for (int row = 0; row < 8; row++)
        {
            if (line & (1 << row))
            {
                OLED_DrawPixel(x + col, y + row, 1);
            }
        }
    }
}
void OLED_DrawString(int x, int y, const char *str)
{
    while (*str)
    {
        OLED_DrawChar(x, y, *str);
        x += 6;
        str++;
    }
}
void OLED_DrawGrid()
{
    for (int x = 0; x < 128; x++)
    {
        if (x % 2 == 0)
            OLED_DrawPixel(x, 41, 1);
    }
    for (int x = 32; x < 128; x += 32)
    {
        for (int y = 20; y < 64; y += 4)
        {
            OLED_DrawPixel(x, y, 1);
        }
    }
}
void OLED_DrawWaveForm()
{
    OLED_Clear();
    OLED_DrawGrid();
    int offset = 0;
    float gain = 1.0;
    uint16_t raw_max = 0;
    uint16_t raw_min = 4095;
    for (int i = 0; i < 128; i++)
    {
        uint16_t val = adc_val[i * 8];
        if (val > raw_max)
            raw_max = val;
        if (val < raw_min)
            raw_min = val;
    }
    float v_max_volt = (raw_max * 3.3) / 4095.0;
    float v_min_volt = (raw_min * 3.3) / 4095.0;
    float v_pp = v_max_volt - v_min_volt;

    char str_vmax[20];
    char str_vpp[20];

    sprintf(str_vmax, "MAX:%.2fV", v_max_volt);
    sprintf(str_vpp, "VPP:%.2fV", v_pp);

    OLED_DrawString(0, 0, str_vmax);
    OLED_DrawString(0, 10, str_vpp);

    for (int x = 0; x < 128; x++)
    {
        uint16_t sample = adc_val[x * 8];
        int y = 63 - (int)((float)sample * gain * 43.0 / 4095.0);
        if (y < 20)
            y = 20;
        if (y > 63)
            y = 63;
        OLED_DrawPixel(x, y, 1);
    }
    OLED_Update();
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
void USART_Recieved()
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
                    uint8_t high_byte = data[k];
                    uint8_t low_byte = data[k + 1];
                    high_byte = high_byte & 0x0F;
                    adc_val[adc_idx] = (high_byte << 8) | low_byte;
                    adc_idx++;
                }
                // for (int i = 0; i < 2048; i += 2)
                // {
                //     float filter = Firfilter((float)adc_val[i]);
                //     adc_val[i] = (uint16_t)filter;
                // }
                printf("ADC Values: %d -> %d \n", adc_val[0], adc_val[1023]);
                OLED_DrawWaveForm();
            }
            else
            {
                printf("Error: Frame's length: %d\n", data_len);
            }
        }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 5)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Reconnecting to Wifi the %d/5 time", s_retry_num);
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Recieved IP from Router: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Starting connect to Wifi........");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Successful connecting to Wifi");
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGE(TAG, "Cannot connect to Wifi");
    }
}

struct async_resp_arg
{
    httpd_handle_t hd;
    int fd;
    uint16_t *data_ptr;
};

static void
ws_async_send(void *arg)
{
    struct async_resp_arg *resp_arg = arg;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)resp_arg->data_ptr;
    ws_pkt.len = 1024 * 2;
    ws_pkt.type = HTTPD_WS_TYPE_BINARY;

    httpd_ws_send_frame_async(resp_arg->hd, resp_arg->fd, &ws_pkt);
    free(resp_arg);
}

static esp_err_t trigger_async_send(uint16_t *data_to_send)
{
    if (global_server == NULL || current_ws_fd == -1)
    {
        return ESP_FAIL;
    }
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    if (resp_arg == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    resp_arg->hd = global_server;
    resp_arg->fd = current_ws_fd;
    resp_arg->data_ptr = data_to_send;
    esp_err_t ret = httpd_queue_work(global_server, ws_async_send, resp_arg);
    if (ret != ESP_OK)
    {
        free(resp_arg);
    }
    return ret;
}

static esp_err_t get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    size_t index_html_len = index_html_end - index_html_start;
    return httpd_resp_send(req, (const char *)index_html_start, index_html_len);
}

static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI("WEB", "Web connected !!!");
        current_ws_fd = httpd_req_to_sockfd(req);
    }
    return ESP_OK;
}
static const httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = get_handler, .user_ctx = NULL};
static const httpd_uri_t ws = {.uri = "/ws", .method = HTTP_GET, .handler = ws_handler, .user_ctx = NULL, .is_websocket = true};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Registering the ws handler
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &ws);
        global_server = server;
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_sta();
    USART_Config();
    I2C_Config();
    OLED_Init();
    OLED_Clear();
    OLED_Update();
    start_webserver();
    while (1)
    {
        USART_Recieved();
        trigger_async_send(adc_val);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
