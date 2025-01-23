#include "http/Router.h"
#include "esp_spiffs.h"
#include "SpiffsManager.h"

extern Router webRouter;

namespace {
    struct ApiRoutesInitializer {
        ApiRoutesInitializer() {
            webRouter.addRoute("", HTTP_GET, [](httpd_req_t *req) {
                std::unique_ptr<std::string> fileContent = SpiffsManager::getInstance()
                                                           .getFileContent("/index.html");

                if(!fileContent) {
                  httpd_resp_send_404(req);
                  return ESP_FAIL;
                }

                httpd_resp_set_type(req, "text/html");
                httpd_resp_send(req, fileContent->c_str(), static_cast<ssize_t>(fileContent->size()));
                return ESP_OK;
            });

            webRouter.addRoute("styles.css", HTTP_GET, [](httpd_req_t *req) {
                std::unique_ptr<std::string> fileContent = SpiffsManager::getInstance()
                                                           .getFileContent("/styles.css");

                if(!fileContent) {
                  httpd_resp_send_404(req);
                  return ESP_FAIL;
                }

                httpd_resp_set_type(req, "text/css");
                httpd_resp_send(req, fileContent->c_str(), static_cast<ssize_t>(fileContent->size()));
                return ESP_OK;
            });

            webRouter.addRoute("main.js", HTTP_GET, [](httpd_req_t *req) {
                std::unique_ptr<std::string> fileContent = SpiffsManager::getInstance()
                                                           .getFileContent("/main.js");

                if(!fileContent) {
                  httpd_resp_send_404(req);
                  return ESP_FAIL;
                }

                httpd_resp_set_type(req, "application/javascript");
                httpd_resp_send(req, fileContent->c_str(), static_cast<ssize_t>(fileContent->size()));
                return ESP_OK;
            });
        }
    };

    static ApiRoutesInitializer initializer;
}