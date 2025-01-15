#ifndef TPUNKT_CREDENTIALS_H
#define TPUNKT_CREDENTIALS_H

#include <cstdint>
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
        Credentials() = default;
        CredentialsType type;
        union
        {
            char buff[ crypto_pwhash_STRBYTES ];
            FixedString<crypto_pwhash_STRBYTES> passkey;
            FixedString<crypto_pwhash_STRBYTES> password;
        };

        TPUNKT_MACROS_STRUCT(Credentials);
    };

} // namespace tpunkt

#endif // TPUNKT_CREDENTIALS_H