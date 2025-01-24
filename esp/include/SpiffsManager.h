#include "Logger.h"
#include "esp_spiffs.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H

class SpiffsManager : protected Logger {
public:
    static SpiffsManager& getInstance() {
        static SpiffsManager instance;
        return instance;
    }

    bool init();

    std::unique_ptr<std::string> getFileContent(const std::string& filePath);

private:

    SpiffsManager() : Logger("SpiffsManager"){}

};
#endif //SPIFFSMANAGER_H
