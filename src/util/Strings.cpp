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

void Base32Encode(const char* pin, const size_t ilen, char* pout, const size_t olen)
{
    constexpr int INPUT_BYTES = 5;
    constexpr char BASE32_TABLE[ 32 ] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                                         'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                         'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'};

    const auto chunks = ilen / INPUT_BYTES;
    uint8_t bytes[ INPUT_BYTES ]{};
    size_t resultPos = 0;
    for(size_t i = 0; i < chunks; i++)
    {
        memcpy(bytes, pin + (i * INPUT_BYTES), INPUT_BYTES);
        uint8_t bits[ 8 ]{};
        // First 5 bits
        bits[ 0 ] = (bytes[ 0 ] & 0b11111000) >> 3;

        // Last 3 of first and first 2 of second bytes
        bits[ 1 ] = ((bytes[ 0 ] & 0b00000111) << 2) | ((bytes[ 1 ] & 0b11000000) >> 6);

        // Bits 2-7 of 2nd byte
        bits[ 2 ] = (bytes[ 1 ] & 0b00111110) >> 1;

        // Last bit of the 3rd and first 4 bits of the 4th byte
        bits[ 3 ] = ((bytes[ 1 ] & 0b00000001) << 4) | ((bytes[ 2 ] & 0b11110000) >> 4);

        // ...
        bits[ 4 ] = ((bytes[ 2 ] & 0b00001111) << 1) | ((bytes[ 3 ] & 0b10000000) >> 7);

        bits[ 5 ] = (bytes[ 3 ] & 0b01111100) >> 2;

        bits[ 6 ] = ((bytes[ 3 ] & 0b00000011) << 3) | ((bytes[ 4 ] & 0b11100000) >> 5);

        bits[ 7 ] = bytes[ 4 ] & 0b00011111;

        for(const auto bit : bits)
        {
            if(resultPos >= olen)
            {
                pout[ olen - 1 ] = '\0';
                return;
            }
            pout[ resultPos++ ] = BASE32_TABLE[ bit ];
        }
    }
}

void Base32Decode(const char* pin, const size_t ilen, char* pout, const size_t olen)
{
    constexpr size_t INPUT_BYTES = 8;
    auto base32ToChar = [](const char c)
    {
        if(c >= 'A' && c <= 'Z')
        {
            return c - 'A';
        }
        if(c >= '2' && c <= '7')
        {
            return c - '2' + 26;
        }
        return 0;
    };

    const auto chunks = ilen / INPUT_BYTES;
    size_t resultPos = 0;
    for(size_t i = 0; i < chunks; i++)
    {
        uint8_t bits[ INPUT_BYTES ]{};
        memcpy(bits, pin + (i * INPUT_BYTES), INPUT_BYTES);

        for(unsigned char & bit : bits)
        {
            bit = base32ToChar(bit);
        }

        // 1 1 1 1 1|1 1 1 1 1|1 1 1 1 1|1 1 1 1 1|1 1 1 1 1|1 1 1 1 1|1 1 1 1 1|1 1 1 1 1|
        // 1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|

        uint8_t bytes[ 5 ]{};

        // 00011111 00011111
        bytes[ 0 ] = (bits[ 0 ] << 3) | (bits[ 1 ] >> 2);

        bytes[ 1 ] = (bits[ 1 ] << (3 + 3)) | (bits[ 2 ] << 1) | (bits[ 3 ] >> 4);

        bytes[ 2 ] = (bits[ 3 ] << (3 + 1)) | (bits[ 4 ] >> 1);

        bytes[ 3 ] = (bits[ 4 ] << (3 + 4) | (bits[ 5 ] << 2) | (bits[ 6 ] >> 3));

        bytes[ 4 ] = (bits[ 6 ] << (3 + 2)) | (bits[ 7 ]);

        for(size_t j = 0; j < 5; ++j)
        {
            pout[ resultPos++ ] = bytes[ j ];
        }
    }
    pout[ resultPos ] = '\0';
}

} // namespace tpunkt