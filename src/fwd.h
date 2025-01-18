#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

#include <cstddef>
#include "config.h"

namespace tpunkt
{
    struct User;
    struct File;
    struct Credentials;
    struct AuthToken;
    struct UserStorage;
    struct SessionStorage;
    template <typename T>
    struct SecureBox;
    template <typename T>
    struct SecureList;
    struct InstanceConfig;

    // Fixed string typedefs
    template <size_t len>
    struct FixedString;
    using SessionID = FixedString<TPUNKT_CRYPTO_SESSION_ID_LEN>;
    using UserName = FixedString<TPUNKT_STORAGE_USER_LEN>;
    using UserAgentString = FixedString<50>;


} // namespace tpunkt

#endif // TPUNKT_FWD_H