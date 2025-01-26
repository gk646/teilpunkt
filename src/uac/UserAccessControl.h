#ifndef TPUNKT_UAC_H
#define TPUNKT_UAC_H

#include "fwd.h"
#include "uac/UserPermissions.h"
#include "datastructures/Spinlock.h"

namespace tpunkt
{
    enum class UACStatus : uint8_t
    {
        INVALID,
        OK,
        ERR_NO_ACCESS,
    };

    // All operations are atomic
    // Permissions are automatically inherited implicity for all child files/dirs - until explicit change
    //
    struct UserAccessControl final
    {
        //===== Lookup =====//

        UACStatus lookupFile(UserID user, PermissionFlag perm, FileID file);

        UACStatus lookupDir(UserID user, PermissionFlag perm, DirectoryID dir);

        //===== Rules =====//

        UACStatus ruleAdd(GroupID group, PermissionFlag perm, DirectoryID dir);
        UACStatus ruleAdd(GroupID group, PermissionFlag perm, FileID file);

        UACStatus ruleAdd(UserID user, PermissionFlag perm, DirectoryID dir);
        UACStatus ruleAdd(UserID user, PermissionFlag perm, FileID file);

        //===== Groups =====//

        UACStatus groupContains(GroupID group, UserID user);

        UACStatus groupAdd(GroupID group, UserID user);

        UACStatus groupRemove(GroupID group, UserID user);

        //===== Data =====//

        UACStatus getUserLimits(UserID user, uint64_t);

        UACStatus getUserGroups(UserID user, Collector<GroupID>& collector);

      private:
        Spinlock uacLock;
    };

    UserAccessControl& GetUAC();

} // namespace tpunkt


#endif // TPUNKT_UAC_H