#include "SpiffsManager.h"

#include <fstream>
#include <memory>
#include <string>
#include "esp_spiffs.h"
#include <functional>

bool SpiffsManager::init() {
    logi("Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = nullptr,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        switch (ret) {
            case ESP_FAIL:
                loge("Failed to mount or format filesystem");
                break;
            case ESP_ERR_NOT_FOUND:
                loge("Failed to find SPIFFS partition");
                break;
            default:
                loge("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
                break;
        }
        return false;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(nullptr, &total, &used);
    if (ret != ESP_OK) {
        loge("Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        logi("Partition size: total: %zu, used: %zu", total, used);
    }

    return true;
}

std::string SpiffsManager::getFileContent(const std::string& filePath) {
    std::string fullPath = "/spiffs" + filePath;
    logi("Opening file: %s", fullPath.c_str());

    std::ifstream file(fullPath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        loge("Die Datei '%s' konnte nicht geöffnet werden!", filePath.c_str());
        return {};
    }

    // Lesen des Inhalts in einen String
    std::string content;
    content.reserve(1024); // Reserviere initial 1KB, anpassen je nach erwartetem Dateigröße

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    if (size <= 0) {
        loge("Die Datei '%s' ist leer oder konnte nicht gelesen werden!", filePath.c_str());
        return {};
    }
    file.seekg(0, std::ios::beg);

    content.resize(static_cast<size_t>(size));
    if (!file.read(&content[0], size)) {
        loge("Fehler beim Lesen der Datei '%s'", filePath.c_str());
        return {};
    }

    logi("Die Datei '%s' wurde erfolgreich gelesen.", filePath.c_str());
    return content;
}