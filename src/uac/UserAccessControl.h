#ifndef TPUNKT_UAC_H
#define TPUNKT_UAC_H

#include "datastructures/Spinlock.h"
#include "fwd.h"
#include "uac/UserPermissions.h"

namespace tpunkt
{
    enum class UACStatus : uint8_t
    {
        INVALID,
        OK,
        ERR_NO_ACCESS,
    };

    // All operations are atomic
    // Permissions are automatically inherited implicitly for all child files/dirs - until explicitly change
    struct UserAccessControl final
    {
        //===== Lookup =====//

        UACStatus lookupFile(UserID user, PermissionFlag perm, FileID file);

        //===== Rules =====//

        UACStatus ruleAdd(GroupID group, PermissionFlag perm, FileID file);
        UACStatus ruleAdd(UserID user, PermissionFlag perm, FileID file);

        //===== Groups =====//

        UACStatus groupContains(GroupID group, UserID user);

        UACStatus groupAdd(GroupID group, UserID user);

        UACStatus groupRemove(GroupID group, UserID user);

        //===== Data =====//

        UACStatus getUserLimits(UserID user, UserLimits);

        UACStatus getUserGroups(UserID user, Collector<GroupID>& collector);

      private:
        Spinlock uacLock;
    };

    UserAccessControl& GetUAC();

} // namespace tpunkt


#endif // TPUNKT_UAC_H