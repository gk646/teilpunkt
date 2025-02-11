// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_FIXED_STRING_H
#define TPUNKT_FIXED_STRING_H

#include <cstring>
#include <sodium/utils.h>
#include "util/Logging.h"

namespace tpunkt
{
    // Fixed string for names/keys that handles overflow and truncation
    template <size_t length>
    struct FixedString final
    {
        FixedString() = default;

        explicit FixedString(const char* string)
        {
            assign(string, 0);
        }

        template <size_t oLength>
        explicit FixedString(const FixedString<oLength>& other)
        {
            *this = other;
        }

        template <size_t oLength>
        FixedString& operator=(const FixedString<oLength>& other)
        {
            assign(other.c_str(), oLength);
            return *this;
        }

        template <size_t oLength>
        bool operator==(const FixedString<oLength>& other) const
        {
            // Protected against missing 0 terminator
            for(size_t i = 0U; i < length; ++i)
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

        FixedString& operator=(const char* assignString)
        {
            assign(assignString, 0);
            return *this;
        }

        bool operator==(const char* other)
        {
            for(size_t i = 0U; i < length; ++i)
            {
                const auto oChar = other[ i ];

                if(oChar == '\0')
                {
                    return arr[ i ] == oChar;
                }

                if(arr[ i ] != oChar)
                {
                    return false;
                }
            }
            return false; // Other string is longer
        }

        [[nodiscard]] const char* c_str() const
        {
            return arr;
        }

        char* data()
        {
            return arr;
        }

        void clear()
        {
            sodium_memzero(arr, length);
        }

        [[nodiscard]] size_t size() const
        {
            // Protected against missing 0 terminator
            for(size_t i = 0U; i < length + 1U; ++i)
            {
                if(arr[ i ] == '\0')
                {
                    return i;
                }
            }
            LOG_CRITICAL("Missing string terminator");
            return length;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return length;
        }

      private:
        static size_t strlen_limited(const char* str, const size_t limit)
        {
            size_t len = 0U;
            if(limit > 0U)
            {
                while(len < limit && str[ len ] != '\0')
                {
                    ++len;
                }
            }
            else
            {
                while(str[ len ] != '\0')
                {
                    ++len;
                }
            }
            return len;
        }

        void assign(const char* assignString, const size_t maxLen)
        {
            if(assignString == nullptr)
            {
                LOG_ERROR("Null string passed");
                return;
            }

            auto assignLen = strlen_limited(assignString, maxLen);
            if(assignLen > length) [[unlikely]]
            {
                assignLen = length;
            }

            strncpy(arr, assignString, assignLen);
        }

        char arr[ length + 1U ]{};
        static_assert(length > 0U, "Cannot be empty");
    };

} // namespace tpunkt

#endif // TPUNKT_FIXED_STRING_H