#include "WiFiManager.h"
#include <sys/types.h>
#include "Config.h"
#include "http/HttpServer.h"
#include "SpiffsManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sensor/Ultrasonicsensor.h"
#include "StateManager.h"

#define TRIG_PIN GPIO_NUM_15
#define ECHO_PIN GPIO_NUM_4

static Logger logger("Main");

Router apiRouter;
Router webRouter;

extern "C" {
void app_main() {
    if (!SpiffsManager::getInstance().init()) {
        logger.loge("Failed to initialize SPIFFS. Restarting...");
        esp_restart();
    }

    Config &config = Config::getInstance();

    WiFiManager wifiManager;
    wifiManager.connect(config.getWifiSSID(), config.getWifiPassword());

    if (!wifiManager.isConnected()) {
        logger.loge("WiFi connection failed. Restarting...");
        esp_restart();
    }

    HttpServer server;

    server.use("/api", apiRouter);
    server.use("/", webRouter);

    if (!server.start()) {
        logger.loge("Failed to start HTTP server. Restarting...");
        esp_restart();
    }

    UltrasonicSensor sensor(TRIG_PIN, ECHO_PIN);

    if (!StateManager::getInstance().init(&sensor)) {
        logger.loge("Failed to initialize state manager. Restarting...");
        esp_restart();
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}
}
