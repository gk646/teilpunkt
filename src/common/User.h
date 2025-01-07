#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "datastructures/FixedString.h"
#include "uac/UserPermission.h"
#include "config.h"

namespace tpunkt
{
    struct User final
    {
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        FixedString<25> password;
    };

} // namespace tpunkt

#endif //TPUNKT_USER_H