// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_PERMISSION_STORAGE_H
#define TPUNKT_PERMISSION_STORAGE_H

#include <vector>
#include "fwd.h"
#include "uac/UserPermissions.h"

namespace tpunkt
{
    struct GroupFilePermission final
    {
        FileID file;
        FileID parent;
        GroupID group;
        PermissionFlag perms;
    };

    struct UserFilePermission final
    {
        UserID user;
        FileID file;
        PermissionFlag perms;
    };

    struct EndpointContainer final
    {
        std::vector<GroupFilePermission> groups;
        std::vector<UserFilePermission> users;
    };

    struct PermissionStorage final
    {

        bool userHasPermission(UserID user, PermissionFlag perm, FileID file);

        bool groupHasPermission(GroupID group, PermissionFlag perm, FileID file);

      private:
        std::vector<EndpointContainer> endpoints;
    };
} // namespace tpunkt

#endif // TPUNKT_PERMISSION_STORAGE_H