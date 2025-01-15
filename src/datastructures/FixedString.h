#ifndef TPUNKT_FIXED_STRING_H
#define TPUNKT_FIXED_STRING_H

// Fixed string for names/keys that handles overflow and truncation

#include <cstring>
#include "util/Logger.h"

namespace tpunkt
{
    template <size_t size>
    struct FixedString final
    {
        FixedString() = default;

        void assign(const char* assignString)
        {
            if(assignString == nullptr)
            {
                LOG_ERROR("Null string passed");
            }

            auto assignLen = strlen(assignString);
            if(assignLen + 1 > size)
            {
                LOG_WARNING("String is too long and will be truncated");
                assignLen = size - 1;
            }

            memcpy(data, assignString, assignLen);
            data[ assignLen + 1 ] = '\0';
        }

        void assign(const char* assignString, int assignLen)
        {
            if(assignString == nullptr)
            {
                LOG_ERROR("Null string passed");
            }
            if(assignLen + 1 > size)
            {
                LOG_WARNING("String is too long and will be truncated");
                assignLen = size - 1;
            }

            memcpy(data, assignString, assignLen);
            data[ assignLen + 1 ] = '\0';
        }

        [[nodiscard]] const char* get() const
        {
            return data;
        }

      private:
        char data[ size + 1 ];
        TPUNKT_MACROS_STRUCT(FixedString);
    };

} // namespace tpunkt


#endif // TPUNKT_FIXED_STRING_H