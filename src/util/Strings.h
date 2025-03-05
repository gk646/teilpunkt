// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_STRINGS_H
#define TPUNKT_STRINGS_H

#include <cstddef>
#include <cstdint>
#include "fwd.h"

namespace tpunkt
{
// Transfers the given number into a string
bool NumberToString(char* buf, size_t len, uint32_t num);
// Appends additional text
bool NumberToStringEx(char* buf, size_t len, uint32_t num, const char* add);

bool IsValidFilename(const FileName& name);

} // namespace tpunkt

#endif // TPUNKT_STRINGS_H