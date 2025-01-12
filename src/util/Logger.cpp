#include <cstdio>
#include <cstdarg>

#include "util/Logger.h"

namespace tpunkt
{
    static Logger LOGGER{};

    void Logger::init()
    {
        /*
        try
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::debug);

            std::vector<spdlog::sink_ptr> sinks = {console_sink};
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("teilpunkt.log", true);
            file_sink->set_level(spdlog::level::debug);
            sinks.push_back(file_sink);

            auto logger = std::make_shared<spdlog::logger>("", sinks.begin(), sinks.end());
            spdlog::set_default_logger(logger);
            spdlog::set_level(spdlog::level::info);
            spdlog::flush_on(spdlog::level::info);
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            fprintf(stderr, "Log initialization failed:%s", ex.what());
        }
        spdlog::debug("spdlog initialized");
        */
    }

    static const char* GetLevelString(const LogLevel level)
    {
        switch (level)
        {
        case LogLevel::INFO:
            return "\033[1;37m[INF]\033[0m";
        case LogLevel::WARNING:
            return "\033[1;33m[WARN]\033[0m";
        case LogLevel::ERROR:
            return "\033[1;31m[ERR]\033[0m";
        case LogLevel::FATAL:
            return "\033[1;31m[FATAL]\033[0m";
        }
        return "";
    }

    void Logger::log(const LogLevel level, const char* msg, ...)
    {
        FILE* output = (level >= LogLevel::ERROR) ? stderr : stdout;

        fprintf(output, "%s ", GetLevelString(level));

        va_list va_args;
        va_start(va_args, msg);
        vfprintf(output, msg, va_args);
        va_end(va_args);

        fprintf(output, "\n");

        fflush(output);
    }

    void Logger::shutdown() {}

    Logger& GetLogger() { return LOGGER; }
} // namespace tpunkt