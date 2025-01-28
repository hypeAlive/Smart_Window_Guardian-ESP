#pragma once

#include <Logger.h>
#include <string>
#include "esp_http_client.h"
#include "cJSON.h"

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

class HttpClient {
private:
    static Logger logger;
    static bool fetch(const std::string& endpoint, cJSON* json);
public:
    static void registerEsp(const std::string& ip);
    static void sendSaveState();
    static void sendState();

};


#endif //HTTPCLIENT_H
