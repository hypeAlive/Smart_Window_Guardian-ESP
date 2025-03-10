#include "WiFiManager.h"
#include "nvs_flash.h"

EventGroupHandle_t WiFiManager::wifi_event_group = nullptr;

void WiFiManager::init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID,
        [](void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
            static_cast<WiFiManager*>(arg)->eventHandler(event_base, event_id, event_data);
        },
        this, nullptr
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP,
        [](void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
            static_cast<WiFiManager*>(arg)->eventHandler(event_base, event_id, event_data);
        },
        this, nullptr
    ));

    logi("WiFi initialized.");
}

WiFiManager::~WiFiManager() {
    esp_wifi_stop();
    esp_wifi_deinit();
    vEventGroupDelete(wifi_event_group);
    logi("WiFi deinitialized.");
}

void WiFiManager::connect(const std::string& ssid, const std::string& password) const {
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_config = {};
    strncpy(reinterpret_cast<char*>(wifi_config.sta.ssid), ssid.c_str(), sizeof(wifi_config.sta.ssid));
    strncpy(reinterpret_cast<char*>(wifi_config.sta.password), password.c_str(), sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    logi("Connecting to WiFi: %s", ssid.c_str());

    ESP_ERROR_CHECK(esp_wifi_start());
    logi("WiFi started.");

    ESP_ERROR_CHECK(esp_wifi_connect());

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    logi("Successfully connected to WiFi.");
}

bool WiFiManager::isConnected() const {
    EventBits_t bits = xEventGroupGetBits(wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}

void WiFiManager::eventHandler(esp_event_base_t event_base, int32_t event_id, void* event_data) const {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            logi("WiFi STA started.");
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            logw("Disconnected from WiFi. Retrying...");
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        char ipStr[16];
        snprintf(ipStr, sizeof(ipStr), IPSTR, IP2STR(&event->ip_info.ip));
        logi("Got IP: %s", ipStr);
        ipAddress = std::string(ipStr);
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
