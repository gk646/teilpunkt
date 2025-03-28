// SPDX-License-Identifier: GPL-3.0-only

#include <cctype>
#include <charconv>
#include <datastructures/FixedString.h>
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

    const size_t numLen = static_cast<uint64_t>(result.ptr - buf);
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

bool StringToNumber(const char* buf, const size_t len, uint32_t& num)
{
    auto result = std::from_chars(buf, buf + len, num);
    return result.ec != std::errc();
}

bool IsValidFilename(const FileName& name)
{
    if(name.size() == 0) [[unlikely]]
    {
        return false;
    }
    for(const auto chr : name)
    {
        if(isprint(chr) == 0) [[unlikely]]
        {
            return false; // Not a printable character
        }
    }
    return true;
}

bool IsValidUserName(const UserName& name)
{
    const size_t size = name.size();
    if(size < TPUNKT_AUTH_MIN_USER_LEN || size > TPUNKT_STORAGE_USER_LEN)
    {
        return false;
    }

    for(const auto chr : name)
    {
        if(isalpha(chr) == 0 && isdigit(chr) == 0)
        {
            return false; // Not a character or digit
        }
    }
    return true;
}

bool IsValidPassword(const UserPassword& password)
{
    return password.size() == TPUNKT_AUTH_PASSWORD_LEN;
}

} // namespace tpunkt