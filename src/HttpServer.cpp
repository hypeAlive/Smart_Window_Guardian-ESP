#include "HttpServer.h"
#include "esp_log.h"

bool HttpServer::start() {
    serverHandle = initializeServer();
    if (serverHandle == nullptr) {
        ESP_LOGE(getTag(), "Failed to start HTTP server");
        return false;
    }

    ESP_LOGI(getTag(), "HTTP server started successfully");
    return true;
}

void HttpServer::stop() {
    if (serverHandle) {
        httpd_stop(serverHandle);
        ESP_LOGI(getTag(), "HTTP server stopped");
        serverHandle = nullptr;
    }
}

httpd_handle_t HttpServer::initializeServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = nullptr;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(getTag(), "Error starting server!");
        return nullptr;
    }

    httpd_uri_t rootUri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) -> esp_err_t {
            // user_ctx für Zugriff auf die Instanz nutzen
            auto *server = static_cast<HttpServer *>(req->user_ctx);
            return server->handleRoot(req);
        },
        .user_ctx = this

    };

    if (httpd_register_uri_handler(server, &rootUri) != ESP_OK) {
        ESP_LOGE(getTag(), "Failed to register root URI handler");
        httpd_stop(server);
        return nullptr;
    }

    return server;
}

esp_err_t HttpServer::handleRoot(httpd_req_t *req) {
    ESP_LOGI(getTag(), "Handling root route, serving index.html from SPIFFS");

    // Öffne die Datei "index.html" aus dem SPIFFS-Dateisystem
    FILE* f = fopen("/spiffs/index.html", "r");
    if (f == NULL) {
        ESP_LOGE(getTag(), "Failed to open /spiffs/index.html");
        // Falls Öffnen fehlschlägt, sende eine Fehlermeldung
        const char *error_response =
            "<!DOCTYPE html>"
            "<html>"
            "<head><title>Error</title></head>"
            "<body><h1>Failed to open index.html</h1></body>"
            "</html>";
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, error_response, HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    // Puffer zum Lesen des Inhalts der Datei
    char buf[1024] = { 0 };
    size_t bytes_read = fread(buf, 1, sizeof(buf) - 1, f); // Lese die Datei
    fclose(f);  // Datei schließen
    if (bytes_read == 0) {
        ESP_LOGE(getTag(), "index.html is empty or could not be read");
        // Falls keine Daten gelesen wurden, Fehler zurückgeben
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read index.html");
        return ESP_FAIL;
    }

    // Sende den Inhalt von index.html an den HTTP-Client
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, buf, bytes_read);

    ESP_LOGI(getTag(), "Served index.html successfully");
    return ESP_OK;
}