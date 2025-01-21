#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_http_server.h"

class HttpServer {
public:
    HttpServer();

    bool start();

    void stop();

private:
    httpd_handle_t serverHandle;

    static esp_err_t handleRoot(httpd_req_t *req);

    httpd_handle_t initializeServer();
};

#endif // HTTP_SERVER_H