#ifndef TPUNKT_LOGGER_H
#define TPUNKT_LOGGER_H

#include <cstdint>
#include "util/Macros.h"

namespace tpunkt
{
    enum class LogLevel : uint8_t
    {

        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    struct Logger
    {
        Logger() = default;
        void init();
        void log(LogLevel level, const char* msg, ...);
        void shutdown();

    private:
        LogLevel minimalLevel = LogLevel::INFO;
        TPUNKT_MACROS_STRUCT(Logger);
    };

    Logger& GetLogger();

} // namespace tpunkt

#define LOG_INFO(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::INFO, msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::WARNING, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::ERROR, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::FATAL, msg, ##__VA_ARGS__)


#endif //TPUNKT_LOGGER_H