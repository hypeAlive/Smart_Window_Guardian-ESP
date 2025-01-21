#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

void app_main() {
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}