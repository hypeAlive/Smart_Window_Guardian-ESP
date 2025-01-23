#include "http/Router.h"

extern Router apiRouter;

namespace {
    struct ApiRoutesInitializer {
        ApiRoutesInitializer() {
            apiRouter.addRoute("", HTTP_GET, [](httpd_req_t *req) {
                const char *html_response = R"(
                    <!DOCTYPE html>
                    <html>
                    <head>
                    <title>ESP32 HTTP Server</title>
                    </head>
                    <body>
                    <h1>Willkommen beim ESP32 HTTP-Server!</h1>
                    <p>Der Server läuft erfolgreich.</p>
                    <a href="/api/state">Status prüfen</a>
                    </body>
                    </html>
                    )";

                httpd_resp_set_type(req, "text/html");
                httpd_resp_send(req, html_response, HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
              });
            apiRouter.addRoute("/state", HTTP_GET, [](httpd_req_t *req) {
                const char *json_response = R"({"state": "ok"})";
                httpd_resp_set_type(req, "application/json");
                httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            });
        }
    };

    static ApiRoutesInitializer initializer;
}