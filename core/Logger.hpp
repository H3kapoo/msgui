#pragma once

#include <cstdint>
#include <string>

namespace msgui
{
/* Simple logger class, nothing more */
class Logger
{
public:
    // Internal Defs
    enum Level : uint8_t
    {
        ERROR   = (1 << 0),
        INFO    = (1 << 1),
        WARNING = (1 << 2),
        DEBUG   = (1 << 3),
        ALL     = ERROR | INFO | WARNING | DEBUG
    };

public:
    Logger() = default;
    Logger(const std::string name, const std::string& outputFile = "");

    // Normal
    void raw(const char* format, ...) const;
    void info(const char* format, ...) const;
    void infoLn(const char* format , ...) const;
    void debug(const char* format, ...) const;
    void debugLn(const char* format, ...) const;
    void warn(const char* format, ...) const;
    void warnLn(const char* format, ...) const;
    void error(const char* format, ...) const;
    void errorLn(const char* format, ...) const;

    // Getters
    const std::string& getName() const;

    // Static Setters
    static void setLevels(const uint8_t level);
    static void unsetLevels(const uint8_t level);

private:
    std::string name_{"?"};
    static uint8_t allowedLevels;
};
} // namespace msgui