#include "spi.h"
#include "driver/spi_master.h"

spi_device_handle_t spi;

void SPI_Config()
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320 * 320 * 2,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));
}
void SPI_Send(uint8_t data_to_send)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 8;

    t.flags = SPI_TRANS_USE_TXDATA;
    t.tx_data[0] = data_to_send;

    t.rx_buffer = NULL;

    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}