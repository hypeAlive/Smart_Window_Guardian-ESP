#include "http/Router.h"

extern Router webRouter;

namespace {
    struct ApiRoutesInitializer {
        ApiRoutesInitializer() {
            webRouter.addRoute("", HTTP_GET, [](httpd_req_t *req) {
                const char *json_response = R"({})";
                httpd_resp_set_type(req, "application/json");
                httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            });
        }
    };

    static ApiRoutesInitializer initializer;
}