#include <cJSON.h>

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
#include "http/HttpClient.h"

#define TRIG_PIN GPIO_NUM_15
#define ECHO_PIN GPIO_NUM_4

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 512

static Logger logger("Main");

Router apiRouter;
Router webRouter;

extern const unsigned char jsonplaceholder_cert[] asm("_binary_cert_pem_start");

void http_perform_as_stream_reader(void)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};   // Buffer to store response of http request
    int content_length = 0;
    esp_http_client_config_t config = {
        .url = "https://jsonplaceholder.typicode.com/todos/1",
        .cert_pem = (const char *)jsonplaceholder_cert,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET Request
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE("TAG", "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE("TAG", "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI("TAG", "HTTP GET Status = %d, content_length = %" PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOG_BUFFER_CHAR("TAG", output_buffer, data_read);
            } else {
                ESP_LOGE("TAG", "Failed to read response");
            }
        }
    }
    esp_http_client_close(client);
}

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
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

}
}
