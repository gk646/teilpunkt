#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "fwd.h"
#include "datastructures/FixedString.h"
#include "auth/Credentials.h"

namespace tpunkt
{
    struct User final
    {
        UserName name;
        Credentials credentials;
        bool loggedIn;
    };

} // namespace tpunkt

#endif // TPUNKT_USER_H