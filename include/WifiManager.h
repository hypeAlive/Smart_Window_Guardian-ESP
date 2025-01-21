#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <string>
#include "esp_wifi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <cstring>

/**
 * @class WiFiManager
 * @brief Manages WiFi connections for the ESP32.
 *
 */
class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();

    /**
     * @brief Connects to a WiFi network using the provided credentials.
     *
     * @param ssid The name of the WiFi network (SSID) to connect to.
     * @param password The password associated with the WiFi network.
     */
    void connect(const std::string& ssid, const std::string& password);

    /**
     * @brief Checks if the device is connected to a WiFi network.
     *
     * @return true if the device is connected to a WiFi network.
     * @return false if the device is not connected to any WiFi network.
     */
    bool isConnected();

private:
    static void eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    static EventGroupHandle_t wifi_event_group;
    static const int WIFI_CONNECTED_BIT = BIT0;

    static const char* TAG;
};

#endif // WIFIMANAGER_H