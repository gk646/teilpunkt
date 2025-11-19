// SPDX-License-Identifier: GPL-3.0-only

#include "PermissionStorage.h"

namespace tpunkt
{
bool PermissionStorage::userHasPermission(const UserID user, PermissionFlag perm, const FileID file)
{
    /*
    for(const auto& entry : userFiles)
    {
        if(entry.user == user && entry.file == file)
        {
            if((static_cast<std::underlying_type_t<UserID>>(entry.perms) &
                static_cast<std::underlying_type_t<UserID>>(perm)) != 0U)
            {
                return true;
            }
        }
    }
    */
}

bool PermissionStorage::groupHasPermission(GroupID group, PermissionFlag perm, FileID file)
{
}
} // namespace tpunkt
