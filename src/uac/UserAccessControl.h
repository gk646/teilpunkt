#ifndef TPUNKT_UAC_H
#define TPUNKT_UAC_H

#include "common/User.h"
#include "uac/UserAction.h"

namespace tpunkt
{
    struct UserAccessControl final
    {
        [[nodiscard]] bool hasPermission(const User& user, const UserAction& action) const;
    };

    UserAccessControl& getUserAccessControl();

} // namespace tpunkt


#endif //TPUNKT_UAC_H