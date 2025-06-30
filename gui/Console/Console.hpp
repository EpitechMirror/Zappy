 #pragma once

#include <string>
#include <mutex>

class Console {
    public:
        enum class LogLevel {
            Info,
            Warning,
            Error,
            Debug
        };

        static void setDebugMode(bool mode);

        static void info(const std::string &message);
        static void warning(const std::string &message);
        static void error(const std::string &message);
        static void debug(const std::string &message);
    private:
        static void log(const std::string &message, LogLevel level);

        static bool debugMode_;
        static std::mutex logMutex_;
        static const std::string logLevelToString_(LogLevel level);
};
