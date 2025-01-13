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
        CredentialsType type{};
        union
        {
            struct
            {
                FixedString<crypto_pwhash_STRBYTES> hashedPassword;
            } passkey;

            struct
            {
                FixedString<crypto_pwhash_STRBYTES> hashedPassword;
            } password{};
        };

        Credentials() = default;

        TPUNKT_MACROS_STRUCT(Credentials);
    };

} // namespace tpunkt

#endif //TPUNKT_CREDENTIALS_H