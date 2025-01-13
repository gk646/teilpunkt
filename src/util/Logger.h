#ifndef TPUNKT_LOGGER_H
#define TPUNKT_LOGGER_H

#include <cstdint>
#include "util/Macros.h"

namespace tpunkt
{
    enum class LogLevel : uint8_t
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    struct Logger
    {
        Logger();
        ~Logger();

        void setLogLevel(LogLevel level);
        void log(LogLevel level, const char* msg, ...) const;

    private:
        LogLevel minimalLevel = LogLevel::DEBUG;
        TPUNKT_MACROS_STRUCT(Logger);
    };

    Logger& GetLogger();

} // namespace tpunkt

#define LOG_DEBUG(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::DEBUG, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::INFO, msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::WARNING, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::ERROR, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::FATAL, msg, ##__VA_ARGS__)


#endif //TPUNKT_LOGGER_H