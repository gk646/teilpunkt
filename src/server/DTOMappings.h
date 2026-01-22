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
        value.assign(arr.data(), length + 1);
    }
};

template <size_t length>
struct to<JSON, tpunkt::FixedString<length>>
{
    template <auto Opts>
    static void op(tpunkt::FixedString<length>& value, auto&&... args) noexcept
    {
        serialize<JSON>::op<Opts>(value.view(), args...);
    }
};


template <>
struct from<JSON, tpunkt::FileID>
{
    template <auto Opts>
    static void op(tpunkt::FileID& value, auto&&... args)
    {
        std::array<char, 32> arr;
        parse<JSON>::op<Opts>(arr, args...);
        value = tpunkt::FileID::FromString(arr.data());
    }
};

template <>
struct to<JSON, tpunkt::FileID>
{
    template <auto Opts>
    static void op(const tpunkt::FileID& value, auto&&... args) noexcept
    {
        tpunkt::FixedString<32> str = tpunkt::FileID::ToString(value);
        serialize<JSON>::op<Opts>(str, args...);
    }
};

} // namespace glz

#endif // TPUNKT_DTO_MAPPINGS_H
