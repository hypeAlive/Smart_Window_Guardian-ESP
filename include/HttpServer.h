#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_http_server.h"

/**
 * @brief A class for managing an HTTP server.
 *
 */
class HttpServer {
public:
    HttpServer();

    /**
     * @brief Starts the HTTP server.
     *
     * @return true if the server starts successfully, false if an error occurred while starting.
     */
    bool start();

    /**
     * @brief Stops the HTTP server.
     */
    void stop();

private:
    httpd_handle_t serverHandle;

    static esp_err_t handleRoot(httpd_req_t *req);

    httpd_handle_t initializeServer();
};

#endif // HTTP_SERVER_H