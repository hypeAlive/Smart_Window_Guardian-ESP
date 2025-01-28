#include "http/HttpClient.h"

#include <Config.h>
#include <StateManager.h>

Logger HttpClient::logger("HttpClient");

void HttpClient::registerEsp(const std::string& ip) {
    cJSON *json = cJSON_CreateObject();
    if (json == nullptr) return;

    Config &config = Config::getInstance();
    StateManager &stateManager = StateManager::getInstance();

    cJSON_AddStringToObject(json, "id", config.getUniqueId().c_str());
    cJSON_AddStringToObject(json, "ip", ip.c_str());
    cJSON_AddStringToObject(json, "state", stateToString(stateManager.getCurrentState()));
    const char* saveStateStr;
    const SensorSaveState *currentSaveState = stateManager.getCurrentSaveState();
    if (currentSaveState != nullptr) {
        saveStateStr = sensorStateToString(currentSaveState->getState());
    } else {
        saveStateStr = "UNKNOWN";
    }
    cJSON_AddStringToObject(json, "saveState", saveStateStr);


    char *jsonString = cJSON_PrintUnformatted(json);
    if (jsonString == nullptr) {
        cJSON_Delete(json);
        return;
    }

    std::string fullUrl = config.getPublicApiUrl() + "/esp/register";

    esp_http_client_config_t httpConf = {
        .url = fullUrl.c_str(),
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };
    esp_http_client_handle_t client = esp_http_client_init(&httpConf);
    if (client == nullptr) {
        cJSON_Delete(json);
        free(jsonString);
        return;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_set_post_field(client, jsonString, strlen(jsonString));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int statusCode = esp_http_client_get_status_code(client);
        logger.logi("HTTP POST Status = %d, content_length = %d", statusCode, esp_http_client_get_content_length(client));
    } else {
        logger.loge("HTTP POST request failed");
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(json);
    free(jsonString);

}

void HttpClient::sendSaveState() {
    cJSON *json = cJSON_CreateObject();
    if (json == nullptr) return;

    Config &config = Config::getInstance();
    StateManager &stateManager = StateManager::getInstance();

    cJSON_AddStringToObject(json, "id", config.getUniqueId().c_str());
    cJSON_AddStringToObject(json, "state", stateToString(stateManager.getCurrentState()));
    const char* saveStateStr;
    const SensorSaveState* currentSaveState = stateManager.getCurrentSaveState();
    if (currentSaveState != nullptr) {
        saveStateStr = sensorStateToString(currentSaveState->getState());
    } else {
        saveStateStr = "UNKNOWN";
    }
    cJSON_AddStringToObject(json, "saveState", saveStateStr);

    char *jsonString = cJSON_PrintUnformatted(json);
    if (jsonString == nullptr) {
        cJSON_Delete(json);
        return;
    }

    std::string fullUrl = config.getPublicApiUrl() + "/esp/save-state";

    esp_http_client_config_t httpConf = {
        .url = fullUrl.c_str(),
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };
    esp_http_client_handle_t client = esp_http_client_init(&httpConf);
    if (client == nullptr) {
        cJSON_Delete(json);
        free(jsonString);
        return;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_set_post_field(client, jsonString, strlen(jsonString));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int statusCode = esp_http_client_get_status_code(client);
        logger.logi("HTTP POST Status = %d, content_length = %d", statusCode, esp_http_client_get_content_length(client));
    } else {
        logger.loge("HTTP POST request failed");
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(json);
    free(jsonString);

}

void HttpClient::sendState() {
    cJSON *json = cJSON_CreateObject();
    if (json == nullptr) return;

    Config &config = Config::getInstance();
    StateManager &stateManager = StateManager::getInstance();

    cJSON_AddStringToObject(json, "id", config.getUniqueId().c_str());
    cJSON_AddStringToObject(json, "state", stateToString(stateManager.getCurrentState()));

    char *jsonString = cJSON_PrintUnformatted(json);
    if (jsonString == nullptr) {
        cJSON_Delete(json);
        return;
    }

    std::string fullUrl = config.getPublicApiUrl() + "/esp/state";

    esp_http_client_config_t httpConf = {
        .url = fullUrl.c_str(),
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&httpConf);
    if (client == nullptr) {
        cJSON_Delete(json);
        free(jsonString);
        return;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_set_post_field(client, jsonString, strlen(jsonString));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int statusCode = esp_http_client_get_status_code(client);
        logger.logi("HTTP POST Status = %d, content_length = %d", statusCode, esp_http_client_get_content_length(client));
    } else {
        logger.loge("HTTP POST request failed");
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(json);
    free(jsonString);
}


