#include "Config.h"

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

Config::Config()
    :  wifiSsid(WIFI_SSID), wifiPassword(WIFI_PASSWORD) {
}

const std::string& Config::getWifiSSID() const {
    return wifiSsid;
}

const std::string& Config::getWifiPassword() const {
    return wifiPassword;
}