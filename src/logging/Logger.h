#ifndef TPUNKT_LOGGER_H
#define TPUNKT_LOGGER_H

namespace tpunkt
{
    struct Logger
    {
        void init();
    };

    Logger& GetLogger();
} // namespace tpunkt

#endif //TPUNKT_LOGGER_H