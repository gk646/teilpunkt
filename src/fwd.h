#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

#include "config.h"

namespace tpunkt
{
    struct User;
    struct File;
    struct Credentials;
    struct AuthToken;
    struct UserName;
    struct UserStorage;
    struct SessionStorage;
    template <typename T>
    struct SecureBox;
    template <typename T>
    struct SecureList;
    template <int len>
    struct FixedString;
    using SessionID = FixedString<TPUNKT_CRYPTO_SESSION_ID_LEN>;

} // namespace tpunkt

#endif //TPUNKT_FWD_H