#include "sensor/Ultrasonicsensor.h"

#include <algorithm>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

float calculateMedian(std::vector<float>& distances) {
    std::ranges::sort(distances);
    size_t size = distances.size();
    if (size % 2 == 0) {
        // Median für gerade Anzahl Elemente
        return (distances[size / 2 - 1] + distances[size / 2]) / 2.0f;
    } else {
        // Median für ungerade Anzahl Elemente
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
    return (static_cast<float>(duration_us) * 0.0343f) / 2.0f;
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
}

bool UltrasonicSensor::validateResult(float distance) const {
  return distance >= 2.0f && distance <= 400.0f;
}

float UltrasonicSensor::measureDistance() {

    gpio_set_level(triggerPin, 0);
    esp_rom_delay_us(2);
    gpio_set_level(triggerPin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(triggerPin, 0);

    uint32_t start_time = 0;
    uint32_t end_time = 0;

    // start time measurement
    while (gpio_get_level(echoPin) == 0) {
        start_time = esp_timer_get_time();
    }

    // wait until time measurement is done
    while (gpio_get_level(echoPin) == 1) {
        end_time = esp_timer_get_time();
    }

    uint32_t duration = end_time - start_time;

    float distance = calculateDistance(duration);

    if (validateResult(distance)) {
        logi("Measured distance: %.2f cm", distance);
        return distance;
    } else {
        logi("Invalid measurement: Out of range!");
        return -1.0f;
    }
}

