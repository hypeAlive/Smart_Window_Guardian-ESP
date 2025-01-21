#ifndef LOGGERTAG_H
#define LOGGERTAG_H

#include <string>

/**
 * @class LoggerTag
 * @brief Provides a logging tag for derived classes.
 */
class LoggerTag {
public:
    explicit LoggerTag(const std::string& tag) : tag_(tag) {}
    virtual ~LoggerTag() = default;

    /**
     * @brief Retrieves the logger tag.
     * 
     * @return The logger tag as a constant character pointer.
     */
    const char* getTag() const { return tag_.c_str(); }

private:
    std::string tag_;
};

#endif // LOGGERTAG_H