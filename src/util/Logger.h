#ifndef TPUNKT_LOGGER_H
#define TPUNKT_LOGGER_H

#include <cstdint>
#include "util/Macros.h"
#include "monitoring/EventMonitor.h"

namespace tpunkt
{
    enum class LogLevel : uint8_t
    {
        DEBUG,    // Debug message
        INFO,     // Info/Event message
        WARNING,  // Unusual behavior
        ERROR,    // No shutdown error
        CRITICAL, // Graceful shutdown error
        FATAL     // Immediate shutdown error (runtime assert)
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
#define LOG_CRITICAL(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::CRITICAL, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::FATAL, msg, ##__VA_ARGS__)

#define STRINGIFY(x) #x

#define LOG_EVENT(type, action, status)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if constexpr(tpunkt::IsWarnEvent(tpunkt::EventStatus::status))                                                 \
        {                                                                                                              \
            LOG_WARNING(#type " : " #action " : " #status);                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            LOG_INFO(#type " : " #action " : " #status);                                                               \
        }                                                                                                              \
        tpunkt::GetEventMonitor().log(tpunkt::EventType::type, tpunkt::EventAction::action,                            \
                                      tpunkt::EventStatus::status);                                                    \
    } while(0)

#endif // TPUNKT_LOGGER_H