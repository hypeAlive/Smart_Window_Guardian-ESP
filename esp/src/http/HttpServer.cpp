#include "http/HttpServer.h"

#include <string>

bool HttpServer::start() {
    logi("Starting...");

    serverHandle = initializeServer();
    if (!serverHandle) {
        loge("Failed to start.");
        return false;
    }

    logi("Started successfully.");
    return true;
}

void HttpServer::stop() {
    if (!serverHandle) {
        logw("Not currently running.");
        return;
    }

    logi("Stopping...");

    esp_err_t res = httpd_stop(serverHandle);

    if (res != ESP_OK) {
        loge("Failed to stop");
        return;
    }

    serverHandle = nullptr;

    logi("Stopped successfully.");
}

httpd_handle_t HttpServer::initializeServer() {
    logi("Initializing...");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    config.server_port = 80;
    config.task_priority = tskIDLE_PRIORITY + 5;
    config.stack_size = 8192;
    config.max_uri_handlers = 10;

    httpd_handle_t server = nullptr;
    esp_err_t res = httpd_start(&server, &config);
    if (res != ESP_OK) {
        loge("Failed to initialize HTTP server.");
        return nullptr;
    }

    if (!registerRoutes(server)) {
        return nullptr;
    }


    logi("Initialized successfully.");
    return server;
}

bool HttpServer::registerRoutes(httpd_handle_t server) {
    if (routers.empty()) {
        logw("No routers to register.");
        return false;
    }

    bool success = true;

    for (auto &entry: routers) {
        const std::string &baseUri = entry.first;
        Router *router = entry.second;

        logi("Registering router with base URI: %s", baseUri.c_str());

        auto routes = router->getRoutes();

        for (const auto &internalRoute: routes) {
            std::string fullPath = baseUri + internalRoute.uri;

            httpd_uri_t rootUri = {
                .uri = fullPath.c_str(),
                .method = internalRoute.method,
                .handler = internalRoute.handler,
                .user_ctx = internalRoute.user_ctx
            };

            logi("Registering URI: %s", fullPath.c_str());

            if (httpd_register_uri_handler(server, &rootUri) != ESP_OK) {
                success = false;
                httpd_stop(server);
                logw("Failed to register URI: %s", fullPath.c_str());
                break;
            }
        }
    }
    return success;
}

void HttpServer::use(const std::string &baseUri, Router &router) {
    routers.emplace_back(baseUri, &router);
}
