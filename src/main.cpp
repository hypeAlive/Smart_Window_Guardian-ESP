#include "WiFiManager.h"
#include <dirent.h>
#include <sys/types.h>
#include "Config.h"
#include "http/HttpServer.h"
#include "SpiffsManager.h"
#include "esp_log.h"

static Logger logger("Main");

Router apiRouter;
Router webRouter;

void initHttpServer() {
    HttpServer server;

    server.use("/api", apiRouter);
    server.use("/", webRouter);

    server.start();
}

extern "C" {
void app_main() {
    SpiffsManager::getInstance().init();

    Config &config = Config::getInstance();

    WiFiManager wifiManager;
    wifiManager.connect(config.getWifiSSID(), config.getWifiPassword());

    if (!wifiManager.isConnected()) {
        printf("WiFi connection failed. Shutting down.\n");
        return;
    }

    initHttpServer();

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("ESP32 is running...\n");
    }
}
}
