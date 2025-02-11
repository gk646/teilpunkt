// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STRINGS_H
#define TPUNKT_STRINGS_H

#include <cstddef>
#include <cstdint>

namespace tpunkt
{
// Transfers the given number into a string
bool NumberToString(char* buf, size_t len, uint32_t num);
// Appends additional text
bool NumberToStringEx(char* buf, size_t len, uint32_t num, const char* add);

} // namespace tpunkt

#endif // TPUNKT_STRINGS_H