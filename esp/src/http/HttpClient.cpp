#include "http/HttpClient.h"

HttpResponse HttpClient::fetch(
    const std::string& url,
    esp_http_client_method_t method,
    cJSON* json_payload,
    const char* content_type
) {

    return HttpResponse(2, nullptr);
}


