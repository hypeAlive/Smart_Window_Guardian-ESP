#ifndef ULTRASOUND_SENSOR_H
#define ULTRASOUND_SENSOR_H

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "Logger.h"

class UltrasonicSensor : protected Logger {
private:
    gpio_num_t triggerPin;
    gpio_num_t echoPin;

    float calculateDistance(uint32_t duration_us) const;

    bool validateResult(float distance) const;

public:
    UltrasonicSensor(gpio_num_t triggerPin, gpio_num_t echoPin)
      : Logger("UltrasonicSensor"), triggerPin(triggerPin), echoPin(echoPin) {
        initPins();
      }

    void initPins();

    float measureDistance();
};

#endif // ULTRASOUND_SENSOR_H