#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "config.h"
#include "datastructures/FixedString.h"
#include "auth/Credentials.h"

namespace tpunkt
{
    using UserName = FixedString<TPUNKT_STORAGE_USER_LEN>;
    struct User final
    {
        UserName name;
        Credentials credentials{};
        bool loggedIn = false;
    };

} // namespace tpunkt

#endif //TPUNKT_USER_H