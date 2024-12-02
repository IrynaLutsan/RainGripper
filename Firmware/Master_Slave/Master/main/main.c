#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

void app_main(void)
{
    lora_init();
    lora_set_frequency(433e6);
    lora_enable_crc();
    uint32_t counter;

    while(1){
        lora_receive(); // put into receive mode
        if(lora_received()) {
            lora_receive_packet((uint8_t*)&counter, sizeof(counter));
            printf("Received:\t%lu\n", counter);
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
