#include <cstdio>
#include <cstdarg>

#include "util/Logger.h"


namespace tpunkt
{
    static Logger LOGGER;

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

    void Logger::log(LogLevel level, const char* msg, ...)
    {
        va_list va_args;
        va_start(va_args, msg);
        vfprintf(stdout, msg, va_args);
        va_end(va_args);
        fflush(stdout);
    }

    void Logger::shutdown() {}

    Logger& GetLogger() { return LOGGER; }
} // namespace tpunkt