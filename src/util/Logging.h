// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_LOGGER_H
#define TPUNKT_LOGGER_H

#include <cstdint>
#include "monitoring/EventMonitor.h"
#include "util/Macros.h"

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

    void setEnableFileLogging(bool value);
    void setLogLevel(LogLevel level);
    void log(LogLevel level, const char* msg, ...) const;
    void logEx(LogLevel level, const char* method, const char* file, int line, const char* msg, ...) const;
    [[noreturn]] void logFatal(const char* method, const char* file, int line, const char* msg, ...) const;

  private:
    LogLevel minimalLevel = LogLevel::INFO;
    bool logToFile = false;
    TPUNKT_MACROS_STRUCT(Logger);
};

Logger& GetLogger();

} // namespace tpunkt

#define LOG_DEBUG(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::DEBUG, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::INFO, msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) tpunkt::GetLogger().log(tpunkt::LogLevel::WARNING, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...)                                                                                            \
    tpunkt::GetLogger().logEx(tpunkt::LogLevel::ERROR, __PRETTY_FUNCTION__, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...)                                                                                         \
    tpunkt::GetLogger().logEx(tpunkt::LogLevel::CRITICAL, __PRETTY_FUNCTION__, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) tpunkt::GetLogger().logFatal(__PRETTY_FUNCTION__, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_EVENT(type, action, status, ...)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if constexpr(IsWarnEvent(EventStatus::status))                                                                 \
        {                                                                                                              \
            LOG_WARNING(#type " : " #action " : " #status);                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            LOG_INFO(#type " : " #action " : " #status);                                                               \
        }                                                                                                              \
        GetEventMonitor().log(EventType::type, EventAction::action, EventStatus::status);                              \
    } while(false)

#endif // TPUNKT_LOGGER_H