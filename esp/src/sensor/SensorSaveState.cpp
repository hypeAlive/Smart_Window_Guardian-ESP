#include "sensor/SensorSaveState.h"
#include <algorithm>

#include "esp_log.h"

const char* sensorStateToString(SensorState state) {
    switch (state) {
        case SensorState::ON: return "ON";
        case SensorState::OFF: return "OFF";
        case SensorState::MID: return "MID";
        default: return "UNKNOWN";
    }
}

const char* SensorSaveState::stateToString() const {
    return sensorStateToString(state);
}

SensorState SensorSaveState::stringToState(const std::string& str) {
    std::string stateStr = str;
    std::transform(stateStr.begin(), stateStr.end(), stateStr.begin(), ::toupper);

    ESP_LOGI("SensorSaveState", "stateStr: %s", stateStr.c_str());

    if (stateStr == "ON") {
        return SensorState::ON;
    }
    if (stateStr == "OFF") {
        return SensorState::OFF;
    }
    if (stateStr == "MID") {
        return SensorState::MID;
    }

    return SensorState::UNKNOWN;
}
