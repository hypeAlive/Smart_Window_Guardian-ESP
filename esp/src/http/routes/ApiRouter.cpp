#include "http/Router.h"
#include "StateManager.h"
#include "sensor/SensorSaveState.h"
#include "cJSON.h"

extern Router apiRouter;

namespace {
    struct ApiRoutesInitializer {
        ApiRoutesInitializer() {
            apiRouter.addRoute("/?", HTTP_GET, [](httpd_req_t *req) {
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

            apiRouter.addRoute("/save-state", HTTP_GET, [](httpd_req_t *req) {
                const SensorSaveState* state = StateManager::getInstance().getCurrentSaveState();

                if (!state) {
                    state = &SensorSaveState::Unknown();
                }

                const char *state_str = state->stateToString();
                char json_response[100];
                snprintf(json_response, sizeof(json_response), R"({"state": "%s"})", state_str);
                httpd_resp_set_type(req, "application/json");
                httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            });

            apiRouter.addRoute("/save-state", HTTP_POST, [](httpd_req_t *req) {
                char buffer[100];
                int ret = httpd_req_recv(req, buffer, sizeof(buffer) - 1);
                if (ret <= 0) {
                    if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                        httpd_resp_send_408(req);
                    }
                    return ESP_FAIL;
                }
                buffer[ret] = '\0';

                ESP_LOGI("API", "Raw JSON buffer: %s", buffer);

                cJSON *json = cJSON_Parse(buffer);
                if (!json) {
                    ESP_LOGE("API", "JSON parsing failed. Raw buffer: %s", buffer);
                    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
                    return ESP_FAIL;
                }

                cJSON *stateJson = cJSON_GetObjectItem(json, "state");
                if (!cJSON_IsString(stateJson)) {
                    cJSON_Delete(json);
                    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing or invalid 'state' field");
                    return ESP_FAIL;
                }

                ESP_LOGI("API", "Extracted state: %s", stateJson->valuestring);

                const char *stateStr = stateJson->valuestring;

                ESP_LOGI("API", "Received state: %s", stateStr);
                SensorState state = SensorSaveState::stringToState(stateStr);
                cJSON_Delete(json);
                if (state == SensorState::UNKNOWN) {
                    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Unknown state value");
                    return ESP_FAIL;
                }

                bool setupSuccess = StateManager::getInstance().setup(state, 5);

                if (setupSuccess) {
                    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
                    return ESP_OK;
                } else {
                    httpd_resp_send(req, "FAIL", HTTPD_RESP_USE_STRLEN);
                    return ESP_FAIL;
                }
            });
        }
    };

    static ApiRoutesInitializer initializer;
}