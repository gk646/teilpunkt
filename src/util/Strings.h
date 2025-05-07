// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_STRINGS_H
#define TPUNKT_STRINGS_H

#include "fwd.h"

namespace tpunkt
{

// Transfers the given number into a string
bool NumberToString(char* buf, size_t len, uint32_t num);

// Appends additional text
bool NumberToStringEx(char* buf, size_t len, uint32_t num, const char* add);

bool StringToNumber(const char* buf, size_t len, uint32_t& num);

bool IsValidFilename(const FileName& name);

bool IsValidUserName(const UserName& name);

bool IsValidPassword(const UserPassword& password);

// Does not support inplace encoding
void Base32Encode(const char* pin, size_t ilen, char* pout, size_t olen);

// Supports inplace decoding
void Base32Decode(const char* pin, size_t ilen, char* pout, size_t olen);

} // namespace tpunkt

#endif // TPUNKT_STRINGS_H