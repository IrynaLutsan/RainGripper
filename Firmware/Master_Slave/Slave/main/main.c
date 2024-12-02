#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "lora.h"

static const char *TAG = "slave";

#define I2C_SLAVE_ADDR	   0x1D
#define TIMEOUT_MS		   1000
#define DELAY_MS		   1000
#define I2C_REG_RAW_DATA   0x14

void configure_i2c(void)
{
    i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = 21,
		.scl_io_num = 22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 100000,
	};
	i2c_param_config(I2C_NUM_0, &conf);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

uint32_t read_rainfall_counter(void);

void app_main(void)
{
    lora_init();
    lora_set_frequency(433e6);
    lora_enable_crc();

    /* Configure the peripheral */
    configure_i2c();

    while (1) {
        uint32_t raw_data = read_rainfall_counter();
        ESP_LOGI(TAG, "Gravity sensor raw data: %lu", raw_data);
        lora_send_packet((uint8_t*)&raw_data, sizeof(raw_data));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

uint32_t read_rainfall_counter(void)
{
    uint8_t reg = I2C_REG_RAW_DATA;
    uint32_t raw_data;
    i2c_master_write_to_device(I2C_NUM_0, I2C_SLAVE_ADDR, &reg, 1, TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_master_read_from_device(I2C_NUM_0, I2C_SLAVE_ADDR, (uint8_t*)&raw_data, sizeof(raw_data), TIMEOUT_MS/portTICK_PERIOD_MS);
    return raw_data;
}