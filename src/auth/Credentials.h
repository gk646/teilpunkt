// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_CREDENTIALS_H
#define TPUNKT_CREDENTIALS_H

#include <sodium/crypto_pwhash.h>
#include "datastructures/FixedString.h"

namespace tpunkt
{
enum class CredentialsType : uint8_t
{
    INVALID,
    PASSWORD,
    PASSKEY
};

struct Credentials final
{
    CredentialsType type = CredentialsType::INVALID;
    UserPassword password;
    UserPasskey passkey;

    bool operator==(const Credentials& other) const
    {
        if(type != other.type)
        {
            return false;
        }
        if(type == CredentialsType::PASSWORD)
        {
            return password == other.password;
        }

        if(type == CredentialsType::PASSKEY)
        {
            return passkey == other.passkey;
        }
        LOG_FATAL("Invalid enum value");
    }
};

} // namespace tpunkt

#endif // TPUNKT_CREDENTIALS_H
