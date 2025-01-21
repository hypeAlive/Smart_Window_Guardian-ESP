#include "WiFiManager.h"
#include <dirent.h>
#include <sys/types.h>
#include "Config.h"
#include "HttpServer.h"

extern "C" {
void app_main() {
    Config& config = Config::getInstance();

    WiFiManager wifiManager;
    wifiManager.connect(config.getWifiSSID(), config.getWifiPassword());

    if (wifiManager.isConnected()) {
        printf("WiFi connected successfully!\n");
    } else {
        printf("WiFi connection failed.\n");
    }

    HttpServer server;

    if (!server.start()) {
        ESP_LOGE("Main", "Failed to start the HTTP server");
        return;
    }


    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("ESP32 is running...\n");
    }
}
}
