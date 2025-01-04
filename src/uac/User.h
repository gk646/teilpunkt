#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "uac/Permissions.h"

namespace tpunkt
{
    struct User final
    {
        const char* name;
        UserPermission perms;
    };

} // namespace tpunkt

#endif //TPUNKT_USER_H