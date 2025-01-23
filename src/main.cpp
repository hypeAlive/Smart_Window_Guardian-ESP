#include "WiFiManager.h"
#include <dirent.h>
#include <sys/types.h>
#include "Config.h"
#include "http/HttpServer.h"
#include "esp_spiffs.h"
#include "esp_log.h"

static Logger logger("Main");

Router apiRouter;
Router webRouter;

void initSpiffs() {
    logger.logi("Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            logger.loge("Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            logger.loge("Failed to find SPIFFS partition");
        } else {
            logger.loge("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        logger.loge("Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        logger.loge("Partition size: total: %d, used: %d", total, used);
    }
}


void initHttpServer() {
    HttpServer server;

    server.use("/api", apiRouter);
    server.use("/", webRouter);

    server.start();
}

extern "C" {
void app_main() {
    initSpiffs();

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
