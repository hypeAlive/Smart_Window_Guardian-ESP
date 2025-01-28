#ifndef CONFIG_H
#define CONFIG_H
#include <string>

/**
 * @class Config
 * @brief The Config class provides safe access to configuration variables from the .env file.
 */
class Config {
public:
    /**
     * @brief Provides the singleton instance of the Config class.
     *
     * @return A reference to the singleton instance of the Config class.
     */
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    /**
     * @brief Retrieves the SSID of the Wi-Fi network.
     *
     * @return A reference to a std::string containing the SSID.
     */
    const std::string& getWifiSSID() const {
        return wifiSsid;
    }

    /**
     * @brief Retrieves the Wi-Fi network password.
     *
     * @return A reference to a std::string containing the Wi-Fi password.
     */
    const std::string& getWifiPassword() const {
        return wifiPassword;
    }

    const std::string& getUniqueId() const {
        return uniqueId;
    }

    const std::string& getPublicApiUrl() const {
        return publicApiUrl;
    }

private:
    Config()
    :  wifiSsid(WIFI_SSID), wifiPassword(WIFI_PASSWORD),
    uniqueId(ESP_UNIQUE_ID), publicApiUrl(PUBLIC_API_URL) {
    }

    std::string wifiSsid;
    std::string wifiPassword;
    std::string uniqueId;
    std::string publicApiUrl;

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif // CONFIG_H