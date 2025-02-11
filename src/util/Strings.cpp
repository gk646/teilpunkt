// SPDX-License-Identifier: Apache License 2.0

#include <charconv>
#include "util/Strings.h"

namespace tpunkt
{

    bool NumberToString(char* buf, const size_t len, const uint32_t num)
    {
        if((buf == nullptr) || len == 0) [[unlikely]]
        {
            return false;
        }

        const auto result = std::to_chars(buf, buf + len - 1, num);
        if(result.ec != std::errc()) [[unlikely]]
        {
            return false;
        }

        *result.ptr = '\0';
        return true;
    }

    bool NumberToStringEx(char* buf, const size_t len, const uint32_t num, const char* add)
    {
        if((buf == nullptr) || (add == nullptr) || len == 0) [[unlikely]]
        {
            return false;
        }

        const auto result = std::to_chars(buf, buf + len - 1, num);
        if(result.ec != std::errc()) [[unlikely]]
        {
            return false;
        }

        const size_t numLen = result.ptr - buf;
        size_t addLen = 0;

        while(add[ addLen ] != '\0')
        {
            if(numLen + addLen >= len - 1)
            {
                return false;
            }
            buf[ numLen + addLen ] = add[ addLen ];
            addLen++;
        }

        buf[ numLen + addLen ] = '\0';
        return true;
    }

} // namespace tpunkt