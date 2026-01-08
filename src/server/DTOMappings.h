// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_DTO_MAPPINGS_H
#define TPUNKT_DTO_MAPPINGS_H

#include <string_view>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>

namespace glz
{

template <size_t length>
struct from<JSON, tpunkt::FixedString<length>>
{
    template <auto Opts>
    static void op(tpunkt::FixedString<length>& value, auto&&... args)
    {
        std::array<char, length + 1> arr;
        parse<JSON>::op<Opts>(arr, args...);
        value.assign(arr.data(), length);
    }
};

template <size_t length>
struct to<JSON, tpunkt::FixedString<length>>
{
    template <auto Opts>
    static void op(tpunkt::FixedString<length>& value, auto&&... args) noexcept
    {
        std::string_view str(value.c_str(), value.size());
        serialize<JSON>::op<Opts>(str, args...);
    }
};

static_assert(sizeof(tpunkt::FileID) <= sizeof(uint64_t), "FileID too big!");

template <>
struct from<JSON, tpunkt::FileID>
{
    template <auto Opts>
    static void op(tpunkt::FileID value, auto&&... args)
    {
        uint64_t num{};
        parse<JSON>::op<Opts>(num, args...);
        memcpy(&value, &num, sizeof(uint64_t));
    }
};

template <>
struct to<JSON, tpunkt::FileID>
{
    template <auto Opts>
    static void op(tpunkt::FileID& value, auto&&... args) noexcept
    {
        uint64_t num{};
        memcpy(&value, &num, sizeof(uint64_t));
        serialize<JSON>::op<Opts>(num, args...);
    }
};

} // namespace glz

#endif // TPUNKT_DTO_MAPPINGS_H
