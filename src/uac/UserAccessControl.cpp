// SPDX-License-Identifier: Apache License 2.0

#include <uac/UserAccessControl.h>

namespace tpunkt
{

UACStatus UserAccessControl::userCanCreate(UserID user, FileID dir, const FileCreationInfo& info)
{
    // TODO
    // here we need access to the root that contains this file/dir - so we can deduce permissionts
    // otherwise its an expensive lookup
}

UACStatus UserAccessControl::userCanWrite(UserID user, FileID file, uint64_t newSize)
{
}


} // namespace tpunkt