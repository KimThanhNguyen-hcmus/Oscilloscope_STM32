#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "oled.h"
#include "usart.h"
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
uint16_t adc_val[1024];

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

float b[N] = {
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625};

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
// Schedule
void uart_processing_task(void *pvParameters)
{
    int frame_count = 0;
    TickType_t last_time = xTaskGetTickCount();
    while (1)
    {

        bool is_frame_ready = USART_Recieved();
        if (is_frame_ready == true)
        {

            trigger_async_send(adc_val);
            OLED_DrawWaveForm();

            frame_count++;
            TickType_t current_time = xTaskGetTickCount();

            if (current_time - last_time >= pdMS_TO_TICKS(1000))
            {
                ESP_LOGI("FPS_TEST", "Toc do nhan & gui: %d khung hinh/giay", frame_count);
                frame_count = 0;
                last_time = current_time;
            }
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
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
    start_webserver();
    USART_Config();
    I2C_Config();
    OLED_Init();
    OLED_Clear();
    OLED_Update();
    xTaskCreatePinnedToCore(
        uart_processing_task,
        "USART_Task",
        4096,
        NULL,
        5,
        NULL,
        1);
}
