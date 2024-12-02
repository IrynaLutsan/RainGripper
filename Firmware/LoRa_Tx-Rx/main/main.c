#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "lora.h"
#include "esp_random.h"
#include "bootloader_random.h"

void app_main(void)
{
    esp_timer_early_init();
    lora_init();
    lora_set_frequency(433e6);
    lora_enable_crc();
    //bootloader_random_enable();
    //lora_dump_registers();
    uint64_t counter = 0;
    while(1) {
        int64_t rx_start_time = esp_timer_get_time();
        //uint32_t rnd = esp_random();
        while(1) {
            lora_receive(); // put into receive mode
            if(lora_received()) {
                lora_receive_packet((uint8_t*)&counter, sizeof(counter));
                printf("Received:\t%llu\n", counter);
                counter++;
                vTaskDelay(1000/portTICK_PERIOD_MS);
                lora_send_packet((uint8_t*)&counter, sizeof(counter));
                printf("Sent:\t\t%llu\n", counter);   
                break;
            }
            
            int64_t current_time = esp_timer_get_time();
        
            if (current_time - rx_start_time > 2000000) {
                lora_send_packet((uint8_t*)&counter, sizeof(counter));
                printf("Sent: %llu\n", counter);   
                break;
            }
        }
    }
}