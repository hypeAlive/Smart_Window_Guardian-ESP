#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "esp_http_server.h"
#include "Logger.h"
#include "Router.h"

class HttpServer : protected Logger {
public:
    HttpServer() : Logger("HttpServer"), serverHandle(nullptr) {}

    bool start();

    void stop();

    void use(const std::string &baseUri, Router &router);

private:
    httpd_handle_t serverHandle;

    std::vector<std::pair<std::string, Router *>> routers;

    httpd_handle_t initializeServer();

    bool registerRoutes(httpd_handle_t server);
};

#endif //HTTPSERVER_H