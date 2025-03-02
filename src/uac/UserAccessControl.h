// SPDX-License-Identifier: Apache License 2.0

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
    ERR_INVALID_ARGS, // wrong arguments given
};

// All operations are atomic
// Permissions are automatically inherited implicitly for all child files/dirs - until explicitly changed
struct UserAccessControl final
{
    UserAccessControl();
    ~UserAccessControl();

    //===== Lookup =====//

    UACStatus userCanWrite(UserID user, FileID file, uint64_t newSize);

    // Generic action
    UACStatus userCanAction(UserID user, FileID file, PermissionFlag flag);

    //===== Rules =====//

    UACStatus ruleAddFile(GroupID group, PermissionFlag perm, FileID file);
    UACStatus ruleAddDir(GroupID group, PermissionFlag perm, FileID dir);
    UACStatus ruleAddFile(UserID user, PermissionFlag perm, FileID file);
    UACStatus ruleAddDir(UserID user, PermissionFlag perm, FileID dir);

    //===== Groups =====//

    UACStatus groupContains(GroupID group, UserID user);

    UACStatus groupAdd(GroupID group, UserID user);

    UACStatus groupRemove(GroupID group, UserID user);

    //===== Groups =====//

    UACStatus actionAdd(UserID user);

    UACStatus actionRemove(UserID user);

    //===== Limits =====//

    UACStatus limitSetUser(UserID user, const UserLimits& limits);

    UACStatus getUserLimits(UserID user, UserLimits& limits);

    UACStatus getUserGroups(UserID user, Collector<GroupID>& collector);

  private:
    Spinlock uacLock;
};

UserAccessControl& GetUAC();

} // namespace tpunkt


#endif // TPUNKT_UAC_H