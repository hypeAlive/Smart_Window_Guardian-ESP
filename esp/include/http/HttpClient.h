#pragma once

#include <Logger.h>
#include <string>
#include "cJSON.h"

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

class HttpClient {
private:
    static void fetchAsync(const std::string& endpoint, cJSON* json);
public:
    static Logger logger;
    static bool fetch(const std::string& endpoint, cJSON* json);
    static void registerEsp(const std::string& ip);
    static void sendSaveState();
    static void sendState();
    static void sendPing();

};


#endif //HTTPCLIENT_H
