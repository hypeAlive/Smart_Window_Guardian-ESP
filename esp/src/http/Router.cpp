#include "http/Router.h"

void Router::addRoute(std::string path, httpd_method_t method, RequestHandler handler) {

    std::string uri_str = path;

    persistent_uris.push_back(std::unique_ptr<std::string>(new std::string(path)));

    const char *uri_ptr = persistent_uris.back()->c_str();

    httpd_uri_t newRoute = {};
    newRoute.uri = uri_ptr;
    newRoute.method = method;
    newRoute.handler = [](httpd_req_t *req) -> esp_err_t {
        Router::RequestHandler *handler = static_cast<Router::RequestHandler *>(req->user_ctx);
        return (*handler)(req);
    };
    newRoute.user_ctx = new RequestHandler(handler);

    logi("Adding route %s", path.c_str());

    routes.push_back(newRoute);
}
std::vector<httpd_uri_t> Router::getRoutes() {
    return routes;
}