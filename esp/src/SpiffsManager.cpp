#include "SpiffsManager.h"

bool SpiffsManager::init() {
    logi("Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
          loge("Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
          loge("Failed to find SPIFFS partition");
        } else {
          loge("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return false;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        loge("Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        loge("Partition size: total: %d, used: %d", total, used);
    }

    return true;
}

std::unique_ptr<std::string> SpiffsManager::getFileContent(const std::string& filePath) {
    FILE* file = fopen(("/spiffs" + filePath).c_str(), "r");
    if (!file) {
        loge("Die Datei '%s' konnte nicht geöffnet werden!", filePath.c_str());
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    if (fileSize == 0) {
        fclose(file);
        loge("Die Datei '%s' ist leer!", filePath.c_str());
        return nullptr;
    }
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        loge("Speicher für die Datei '%s' konnte nicht reserviert werden!", filePath.c_str());
        return nullptr;
    }

    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';

    std::unique_ptr<std::string> fileContent = std::make_unique<std::string>(buffer);

    free(buffer);
    fclose(file);

    logi("Die Datei '%s' wurde erfolgreich gelesen.", filePath.c_str());
    return fileContent;
}

