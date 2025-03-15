// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_DTO_MAPPINGS_H
#define TPUNKT_DTO_MAPPINGS_H

#include <string_view>
#include <glaze/glaze.hpp>

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
} // namespace glz

template <>
struct glz::meta<tpunkt::Credentials>
{
    using T = tpunkt::Credentials;
    static constexpr auto value = glz::object("password", &T::password, "type", &T::type, "passkey", &T::passkey);
};


#endif // TPUNKT_DTO_MAPPINGS_H