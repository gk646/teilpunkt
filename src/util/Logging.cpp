#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <csignal>
#include "util/Logging.h"

namespace tpunkt
{
    namespace global
    {
        static Logger* Logger;
    }

    Logger::Logger()
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN(Logger);
    }

    Logger::~Logger()
    {
        // Save to file etc.
        LOG_INFO("teilpunkt closed successfully");
        TPUNKT_MACROS_GLOBAL_RESET(Logger);
    }

    void Logger::setEnableFileLogging(const bool value)
    {
        logToFile = value;
    }

    Logger& GetLogger()
    {
        // Special case - logger can get itself
        if(global::Logger != nullptr)
        {
            return *global::Logger;
        }
        (void)fprintf(stderr, "No logger");
        exit(1);
    }

    void Logger::setLogLevel(const LogLevel level)
    {
        minimalLevel = level;
    }

    static const char* GetLevelString(const LogLevel level)
    {
        switch(level)
        {
            case LogLevel::DEBUG:
                return "\033[1;37m[DBG]\033[0m";
            case LogLevel::INFO:
                return "\033[1;37m[INF]\033[0m";
            case LogLevel::WARNING:
                return "\033[1;33m[WARN]\033[0m";
            case LogLevel::ERROR:
                return "\033[1;31m[ERR]\033[0m";
            case LogLevel::CRITICAL:
                return "\033[1;31m[CRIT]\033[0m";
            case LogLevel::FATAL:
                return "\033[1;31m[FATAL]\033[0m";
        }
        return "";
    }

    void Logger::log(const LogLevel level, const char* msg, ...) const
    {
        if(level < minimalLevel)
        {
            return;
        }

        FILE* output = (level >= LogLevel::ERROR) ? stderr : stdout;

        (void)fprintf(output, "%s ", GetLevelString(level));

        va_list va_args;
        va_start(va_args, msg);
        (void)vfprintf(output, msg, va_args);
        va_end(va_args);

        (void)fputc('\n', output);
        (void)fflush(output);

        if(level == LogLevel::CRITICAL)
        {
            (void)raise(SIGTERM);
        }

        if(level == LogLevel::FATAL)
        {
            exit(1);
        }
    }

    void Logger::logEx(const LogLevel level, const char* method, const char* file, const int line, const char* msg,
                       ...) const
    {
        if(level < minimalLevel)
        {
            return;
        }

        FILE* output = stdout;
        if(level >= LogLevel::ERROR)
        {
            output = stderr;
            (void)raise(SIGTRAP);
        }

        (void)fprintf(output, "%s %s:%d\n", method, file, line);
        (void)fprintf(output, "%s ", GetLevelString(level));

        va_list va_args;
        va_start(va_args, msg);
        (void)vfprintf(output, msg, va_args);
        va_end(va_args);

        (void)fputc('\n', output);
        (void)fflush(output);

        if(level == LogLevel::CRITICAL)
        {
            (void)raise(SIGTERM);
        }

        if(level == LogLevel::FATAL)
        {
            exit(1);
        }
    }

    void Logger::logFatal(const char* method, const char* file, int line, const char* msg, ...) const
    {
        (void)raise(SIGTRAP); // Debugbreak
        (void)fprintf(stderr, "%s %s:%d\n", method, file, line);
        (void)fprintf(stderr, "%s ", GetLevelString(LogLevel::FATAL));
        va_list va_args;
        va_start(va_args, msg);
        (void)vfprintf(stderr, msg, va_args);
        va_end(va_args);
        exit(1);
    }


} // namespace tpunkt