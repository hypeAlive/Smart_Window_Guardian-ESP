#include "HttpServer.h"
#include "esp_log.h"

static const char *TAG = "HttpServer";

HttpServer::HttpServer() : serverHandle(nullptr) {}

bool HttpServer::start() {
    serverHandle = initializeServer();
    if (serverHandle == nullptr) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return false;
    }

    ESP_LOGI(TAG, "HTTP server started successfully");
    return true;
}

void HttpServer::stop() {
    if (serverHandle) {
        httpd_stop(serverHandle);
        ESP_LOGI(TAG, "HTTP server stopped");
        serverHandle = nullptr;
    }
}

httpd_handle_t HttpServer::initializeServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = nullptr;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting server!");
        return nullptr;
    }

    httpd_uri_t rootUri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = handleRoot,
        .user_ctx = nullptr
    };

    if (httpd_register_uri_handler(server, &rootUri) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register root URI handler");
        httpd_stop(server);
        return nullptr;
    }

    return server;
}

esp_err_t HttpServer::handleRoot(httpd_req_t *req) {
    const char *response =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>ESP32 Web Server</title>"
        "</head>"
        "<body>"
        "<h1>Hello ESP!</h1>"
        "</body>"
        "</html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}