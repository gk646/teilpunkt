#ifndef TPUNKT_FIXED_STRING_H
#define TPUNKT_FIXED_STRING_H

// Fixed string for names/keys that handles overflow and truncation

#include <cstring>
#include "util/Logger.h"

namespace tpunkt
{
    template <int size>
    struct FixedString final
    {
        char data[size]{};

        FixedString() = default;
        explicit FixedString(const char* string) { assign(string); }

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

        void assign(const char* assignString, int assignLen)
        {
            if (assignString == nullptr)
            {
                LOG_ERROR("Null string passed");
            }
            if (assignLen + 1 > size)
            {
                LOG_WARNING("String is too long and will be truncated");
                assignLen = size - 1;
            }

            memcpy(data, assignString, assignLen);
            data[assignLen + 1] = '\0';
        }

        const char* get() const { return data; }
    };

} // namespace tpunkt


#endif //TPUNKT_FIXED_STRING_H