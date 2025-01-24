#include "http/Router.h"
#include "StateManager.h"

extern Router apiRouter;

namespace {
    struct ApiRoutesInitializer {
        ApiRoutesInitializer() {
            apiRouter.addRoute("", HTTP_GET, [](httpd_req_t *req) {
                httpd_resp_set_type(req, "text/html");
                httpd_resp_send(req, "Window Guardian ESP API", HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
              });

            apiRouter.addRoute("/state", HTTP_GET, [](httpd_req_t *req) {
                State state = StateManager::getInstance().getCurrentState();
                const char *state_str = stateToString(state);
                char json_response[100];
                snprintf(json_response, sizeof(json_response), R"({"state": "%s"})", state_str);
                httpd_resp_set_type(req, "application/json");
                httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            });
        }
    };

    static ApiRoutesInitializer initializer;
}