#ifndef ROUTER_H
#define ROUTER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "esp_http_server.h"
#include "Logger.h"

class Router : public Logger {
public:
    using RequestHandler = std::function<esp_err_t(httpd_req_t *)>;

    explicit Router()
    : Logger("Router") {}

    void addRoute(std::string path, httpd_method_t method, RequestHandler handler);

    std::vector<httpd_uri_t> getRoutes() ;

private:
    std::vector<httpd_uri_t> routes;
    std::vector<std::unique_ptr<std::string>> persistent_uris;
};

#endif //ROUTER_H