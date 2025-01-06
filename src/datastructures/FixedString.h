#ifndef TPUNKT_FIXEDSTRING_H
#define TPUNKT_FIXEDSTRING_H

// Fixed string for names/keys that handles overflow and truncation

#include <spdlog/spdlog.h>
#include "config.h"

namespace tpunkt
{
    template <int size>
    struct FixedString final
    {
        char data[size]{};

        FixedString() = default;
        FixedString(const char* string) { assign(string); }

        void assign(const char* assignString)
        {
            if (assignString == nullptr)
            {
                LOG_ERROR("Null string passed");
            }

            auto assignLen = strlen(assignString);
            if (assignLen + 1 > size)
            {
                LOG_WARNING("String is too long and will be truncated");
                assignLen = size - 1;
            }

            memcpy(data, assignString, assignLen);
            data[assignLen + 1] = '\0';
        }
    };

} // namespace tpunkt


#endif //TPUNKT_FIXEDSTRING_H