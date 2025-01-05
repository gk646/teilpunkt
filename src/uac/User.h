#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "uac/UserPermission.h"

namespace tpunkt
{
    struct User final
    {
        const char* name;
        UserPermissions perms;
    };

} // namespace tpunkt

#endif //TPUNKT_USER_H