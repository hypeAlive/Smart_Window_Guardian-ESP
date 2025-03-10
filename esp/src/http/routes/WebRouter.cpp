#include "http/Router.h"
#include "esp_spiffs.h"
#include "SpiffsManager.h"

extern Router webRouter;

namespace {
    struct ApiRoutesInitializer {
        ApiRoutesInitializer() {
            webRouter.addRoute("?*", HTTP_GET, [](httpd_req_t *req) {
                std::string uri = req->uri;

                if (uri == "/") {
                    uri = "/index.html";
                }

                std::string fileContent = SpiffsManager::getInstance().getFileContent(uri);

                if (fileContent.empty()) {
                    httpd_resp_send_404(req);
                    return ESP_FAIL;
                }

                if (uri.ends_with(".html")) {
                    httpd_resp_set_type(req, "text/html");
                } else if (uri.ends_with(".css")) {
                    httpd_resp_set_type(req, "text/css");
                } else if (uri.ends_with(".js")) {
                    httpd_resp_set_type(req, "application/javascript");
                } else if (uri.ends_with(".ico")) {
                    httpd_resp_set_type(req, "image/x-icon");
                } else {
                    httpd_resp_set_type(req, "application/octet-stream");
                }

                httpd_resp_send(req, fileContent.c_str(), fileContent.size());
                return ESP_OK;
            });
        }
    };

    static ApiRoutesInitializer initializer;
}