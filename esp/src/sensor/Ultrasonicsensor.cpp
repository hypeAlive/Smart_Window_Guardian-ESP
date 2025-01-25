#include "sensor/Ultrasonicsensor.h"

#include <algorithm>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

volatile uint32_t start_time = 0;
volatile uint32_t end_time = 0;
volatile bool measurement_done = false;

void IRAM_ATTR echoISR(void* arg) {
    if (gpio_get_level(static_cast<gpio_num_t>(reinterpret_cast<int>(arg))) == 1) {
        start_time = esp_timer_get_time();
    } else {
        end_time = esp_timer_get_time();
        measurement_done = true;
    }
}

float UltrasonicSensor::measureDistance() {
    measurement_done = false;

    gpio_set_level(triggerPin, 0);
    esp_rom_delay_us(2);
    gpio_set_level(triggerPin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(triggerPin, 0);

    gpio_set_intr_type(echoPin, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(echoPin, echoISR, (void*)echoPin);

    const uint32_t timeout_us = 38000; // wenn länger als nötig -> fail
    uint32_t start_timeout = esp_timer_get_time();

    while (!measurement_done) {
        if ((esp_timer_get_time() - start_timeout) > timeout_us) {
            gpio_isr_handler_remove(echoPin);
            return -1.0f;
        }
    }

    gpio_isr_handler_remove(echoPin);

    uint32_t duration = end_time - start_time;

    float distance = calculateDistance(duration);

    return validateResult(distance) ? distance : -1.0f;
}


float calculateMedian(std::vector<float>& distances) {
    std::ranges::sort(distances);
    size_t size = distances.size();
    if (size % 2 == 0) {
        return (distances[size / 2 - 1] + distances[size / 2]) / 2.0f;
    } else {
        return distances[size / 2];
    }
}

float UltrasonicSensor::measureMedianDistance(uint32_t duration_seconds) {
    uint32_t startTime = esp_timer_get_time();
    uint32_t duration_us = duration_seconds * 1000000;

    std::vector<float> validDistances;

    while ((esp_timer_get_time() - startTime) < duration_us) {
        float distance = measureDistance();
        if (validateResult(distance)) {
            validDistances.push_back(distance);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (validDistances.empty()) {
        logw("UltrasonicSensor", "Keine gültigen Messwerte gefunden.");
        return -1.0f;
    }

    return calculateMedian(validDistances);
}


float UltrasonicSensor::calculateDistance(uint32_t duration_us) const {
    constexpr float conversionFactor = 0.0343f * 0.5f;
    return static_cast<float>(duration_us) * conversionFactor;
}


void UltrasonicSensor::initPins() {
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << triggerPin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << echoPin);
    gpio_config(&io_conf);

    logi("Pins initialized: Trigger(%d), Echo(%d)", triggerPin, echoPin);

    static bool isrInstalled = false;
    if (!isrInstalled) {
        gpio_install_isr_service(0);
        isrInstalled = true;
    }

    gpio_isr_handler_add(echoPin, echoISR, (void *)echoPin);

}

bool UltrasonicSensor::validateResult(float distance) const {
  return distance >= 2.0f && distance <= 400.0f;
}

