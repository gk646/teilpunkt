#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <csignal>
#include "util/Logger.h"

namespace tpunkt
{
    namespace global
    {
        static Logger* Logger;
    }


    static const char* GetLevelString( const LogLevel level )
    {
        switch( level )
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

    void Logger::setLogLevel( const LogLevel level )
    {
        minimalLevel = level;
    }

    void Logger::log( const LogLevel level, const char* msg, ... ) const
    {
        if( level < minimalLevel )
            return;

        FILE* output = ( level >= LogLevel::ERROR ) ? stderr : stdout;

        fprintf( output, "%s ", GetLevelString( level ) );

        va_list va_args;
        va_start( va_args, msg );
        vfprintf( output, msg, va_args );
        va_end( va_args );

        fputc( '\n', output );

        fflush( output );
        if( level == LogLevel::CRITICAL )
            raise( SIGTERM );
        else if( level == LogLevel::FATAL )
            exit( 1 );
    }

    Logger::Logger()
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN( Logger );
    }

    Logger::~Logger()
    {
        TPUNKT_MACROS_GLOBAL_RESET( Logger );
    }

    Logger& GetLogger()
    {
        // Special case - logger can get itself
        if( global::Logger != nullptr )
            return *global::Logger;
        fprintf( stderr, "No logger" );
        exit( 1 );
    }
} // namespace tpunkt