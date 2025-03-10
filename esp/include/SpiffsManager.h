#include "Logger.h"

#include <memory>
#include <string>

#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H

class SpiffsManager : protected Logger {
public:
    static SpiffsManager& getInstance() {
        static SpiffsManager instance;
        return instance;
    }

    bool init();

    std::string getFileContent(const std::string& filePath);

private:

    SpiffsManager() : Logger("SpiffsManager"){}

};
#endif //SPIFFSMANAGER_H
