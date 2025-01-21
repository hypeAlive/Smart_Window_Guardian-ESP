#ifndef CONFIG_H
#define CONFIG_H
#include <string>

class Config {
public:
    static Config& getInstance();

    const std::string& getWifiSSID() const;
    const std::string& getWifiPassword() const;

private:
    Config();

    std::string wifiSsid;
    std::string wifiPassword;

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif // CONFIG_H