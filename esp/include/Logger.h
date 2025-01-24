#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstdarg>
#include "esp_log.h"

class Logger {
public:
    explicit Logger(const std::string& tag) : tag_(tag) {}

    virtual ~Logger() = default;

    void logi(const char* format, ...) const {
        va_list args;
        va_start(args, format);

        char logMessage[512];
        vsnprintf(logMessage, sizeof(logMessage), format, args);

        ESP_LOGI(getTag(), "%s", logMessage);

        va_end(args);
    }

    void logw(const char* format, ...) const {
        va_list args;
        va_start(args, format);

        char logMessage[512];
        vsnprintf(logMessage, sizeof(logMessage), format, args);

        ESP_LOGW(getTag(), "%s", logMessage);

        va_end(args);
    }

    void loge(const char* format, ...) const {
        va_list args;
        va_start(args, format);

        char logMessage[512];
        vsnprintf(logMessage, sizeof(logMessage), format, args);

        ESP_LOGE(getTag(), "%s", logMessage);

        va_end(args);
    }

    void logd(const char* format, ...) const {
        va_list args;
        va_start(args, format);

        char logMessage[512];
        vsnprintf(logMessage, sizeof(logMessage), format, args);

        ESP_LOGD(getTag(), "%s", logMessage);

        va_end(args);
    }

    void logv(const char* format, ...) const {
        va_list args;
        va_start(args, format);

        char logMessage[512];
        vsnprintf(logMessage, sizeof(logMessage), format, args);

        ESP_LOGV(getTag(), "%s", logMessage);

        va_end(args);
    }

    const char* getTag() const { return tag_.c_str(); }

private:
    std::string tag_;
};

#endif // LOGGER_H