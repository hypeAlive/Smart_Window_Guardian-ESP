#pragma once

#include <string>
#include "esp_http_client.h"
#include "cJSON.h"

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

class HttpResponse {
public:
    int status;
    cJSON* body;
    HttpResponse(int status, cJSON* body)
        : status(status), body(body) {}
    ~HttpResponse() {
      if(!body) return;
      cJSON_Delete(body);
    }
};

class HttpClient {
public:

    static HttpResponse get(const std::string& url) {
        return fetch(url, HTTP_METHOD_GET);
    }

    static HttpResponse post(const std::string& url, cJSON* json_payload)  {
        return fetch(url, HTTP_METHOD_POST, json_payload);
    }

private:
    static HttpResponse fetch(
          const std::string& url,
          esp_http_client_method_t method = HTTP_METHOD_GET,
          cJSON* json_payload = nullptr,
          const char* content_type = "application/json"
      );

};


#endif //HTTPCLIENT_H
