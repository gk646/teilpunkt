#ifndef TPUNKT_FIXED_STRING_H
#define TPUNKT_FIXED_STRING_H

// Fixed string for names/keys that handles overflow and truncation

#include <cstring>
#include "util/Logger.h"

namespace tpunkt
{
    template <size_t length>
    struct FixedString final
    {
        FixedString() = default;

        explicit FixedString(const char* string)
        {
            assign(string);
        }

        void assign(const char* assignString)
        {
            if(assignString == nullptr)
            {
                LOG_ERROR("Null string passed");
                arr[ 0 ] = '\0';
                return;
            }

            auto assignLen = strlen(assignString);
            if(assignLen > length)
            {
                LOG_WARNING("String is too long and will be truncated");
                assignLen = length;
            }

            strncpy(arr, assignString, assignLen);
        }

        [[nodiscard]] const char* c_str() const
        {
            return arr;
        }

        char* data()
        {
            return arr;
        }

        [[nodiscard]] size_t size() const
        {
            // Protected against missing 0 terminator
            for(size_t i = 0; i < length; ++i)
            {
                if(arr[ i ] == '\0')
                    return i;
            }
            return length;
        }

        template <size_t oLength>
        bool operator==(const FixedString<oLength>& other) const
        {
            // Protected against missing 0 terminator
            for(size_t i = 0; i < length; ++i)
            {
                if(i < oLength)
                {
                    const auto myChar = arr[ i ];
                    const auto oChar = other.c_str()[ i ];
                    if(myChar != oChar) [[likely]]
                    {
                        return false;
                    }

                    if(myChar == oChar && myChar == '\0')
                    {
                        return true;
                    }
                }
                else
                {
                    return arr[ i ] == '\0'; // Our data is longer - other data must be terminated here and so must we
                }
            }
            return arr[ length ] == other.c_str()[ length ] && arr[ length ] == '\0'; // Same length
        }

        template <size_t oLength>
        FixedString& operator=(const FixedString<oLength>& other)
        {
            assign(other.c_str());
            return *this;
        }

      private:
        char arr[ length + 1 ]{};
        static_assert(length > 0, "Cannot be empty");
    };

} // namespace tpunkt


#endif // TPUNKT_FIXED_STRING_H