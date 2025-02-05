#include "http/HttpClient.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Config.h>
#include <optional>
#include <StateManager.h>
#include <cstring>
#include "esp_http_client.h"

Logger HttpClient::logger("HttpClient");

extern const unsigned char nicolasfritz_cert[] asm("_binary_cert_pem_start");

struct FetchParams {
    std::string endpoint;
    cJSON* json;
};

// Statische Task-Funktion
static void fetchTask(void* params) {
    FetchParams* fetchParams = static_cast<FetchParams*>(params);
    if (fetchParams == nullptr) {
        vTaskDelete(NULL);
        return;
    }

    bool result = HttpClient::fetch(fetchParams->endpoint, fetchParams->json);

    if (result) {
        HttpClient::logger.logi("HTTP-Anfrage erfolgreich in Task.");
    } else {
        HttpClient::logger.loge("HTTP-Anfrage fehlgeschlagen in Task.");
    }

    // Speicher freigeben
    delete fetchParams;

    vTaskDelete(NULL); // Task beenden
}



cJSON* buildJsonObject(const std::optional<std::string>& ip = std::nullopt) {
    cJSON* json = cJSON_CreateObject();
    if (json == nullptr) {
        // Fehlerbehandlung, eventuell Logging
        return nullptr;
    }

    Config& config = Config::getInstance();
    StateManager& stateManager = StateManager::getInstance();

    // "id" hinzufügen
    if (!cJSON_AddStringToObject(json, "id", config.getUniqueId().c_str())) {
        cJSON_Delete(json);
        return nullptr;
    }

    // Optional "ip" hinzufügen
    if (ip.has_value()) {
        if (!cJSON_AddStringToObject(json, "ip", ip->c_str())) {
            cJSON_Delete(json);
            return nullptr;
        }
    }

    // "state" hinzufügen
    std::string stateStr = stateToString(stateManager.getCurrentState());
    if (!cJSON_AddStringToObject(json, "state", stateStr.c_str())) {
        cJSON_Delete(json);
        return nullptr;
    }

    // "saveState" hinzufügen
    const char* saveStateStr = "UNKNOWN";
    const SensorSaveState* currentSaveState = stateManager.getCurrentSaveState();
    if (currentSaveState != nullptr) {
        saveStateStr = sensorStateToString(currentSaveState->getState());
    }
    if (!cJSON_AddStringToObject(json, "saveState", saveStateStr)) {
        cJSON_Delete(json);
        return nullptr;
    }

    return json;
}

void HttpClient::registerEsp(const std::string& ip) {
    cJSON *json = buildJsonObject(ip);
    if (json == nullptr) return;

    fetchAsync("/esp/register", json);
}

void HttpClient::sendSaveState() {
    cJSON *json = buildJsonObject();
    if (json == nullptr) return;
    fetchAsync("/esp/save-state", json);
}

void HttpClient::sendState() {
    cJSON *json = buildJsonObject();
    if (json == nullptr) return;
    fetchAsync("/esp/state", json);
}

void HttpClient::sendPing() {
    cJSON* json = cJSON_CreateObject();
    if (!cJSON_AddStringToObject(json, "id", Config::getInstance().getUniqueId().c_str())) {
        cJSON_Delete(json);
        return;
    }
    fetchAsync("/esp/ping", json);
}

bool HttpClient::fetch(const std::string& endpoint, cJSON* json) {
    if (json == nullptr) {
        logger.loge("JSON-Objekt ist null");
        return false;
    }

    char* jsonString = cJSON_PrintUnformatted(json);
    if (jsonString == nullptr) {
        logger.loge("Fehler beim Konvertieren des JSON-Objekts in einen String");
        return false;
    }

    Config& config = Config::getInstance();
    std::string fullUrl = config.getPublicApiUrl() + endpoint;

    esp_http_client_config_t httpConf = {
        .url = fullUrl.c_str(),
        .cert_pem = (const char *)nicolasfritz_cert,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&httpConf);
    if (client == nullptr) {
        logger.loge("Fehler beim Initialisieren des HTTP-Clients");
        cJSON_Delete(json);
        free(jsonString);
        return false;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, jsonString, strlen(jsonString));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int statusCode = esp_http_client_get_status_code(client);
        int contentLength = esp_http_client_get_content_length(client);
        logger.logi("HTTP POST Status = %d, content_length = %d", statusCode, contentLength);
    } else {
        logger.loge("HTTP POST Anfrage fehlgeschlagen");
        esp_http_client_cleanup(client);
        cJSON_Delete(json);
        free(jsonString);
        return false;
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(json);
    free(jsonString);
    return true;
}

void HttpClient::fetchAsync(const std::string& endpoint, cJSON* json) {
    // Speicher für die Parameter allozieren
    FetchParams* params = new FetchParams{
        .endpoint = endpoint,
        .json = json
    };

    // Task erstellen
    BaseType_t result = xTaskCreate(
        fetchTask,            // Task-Funktion
        "FetchTask",          // Name der Task (für Debugging)
        4096,                 // Stackgröße in Bytes (kann angepasst werden)
        params,               // Parameter an die Task
        tskIDLE_PRIORITY + 1, // Priorität der Task
        NULL                  // Handle der Task (optional)
    );

    if (result != pdPASS) {
        logger.loge("Fehler beim Erstellen der Fetch-Task");
        delete params;
        // Optional: Weitere Fehlerbehandlung
    }
}




