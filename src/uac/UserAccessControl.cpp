// SPDX-License-Identifier: GPL-3.0-only

#include "uac/UserAccessControl.h"
#include "util/Logging.h"
#include "util/Macros.h"

namespace tpunkt
{
namespace global
{
UserAccessControl* UserAccessControl;
}

UserAccessControl::UserAccessControl()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(UserAccessControl);
}
UserAccessControl::~UserAccessControl()
{
    TPUNKT_MACROS_GLOBAL_RESET(UserAccessControl);
}

UserAccessControl& GetUAC()
{
    TPUNKT_MACROS_GLOBAL_GET(UserAccessControl);
}

UACStatus UserAccessControl::userCanWrite(UserID user, FileID file, uint64_t newSize)
{
    SpinlockGuard guard{uacLock};


    return UACStatus::OK;
}

UACStatus UserAccessControl::userCanAction(UserID user, FileID file, PermissionFlag flag)
{
    SpinlockGuard guard{uacLock};
    // TODO
    // here we need access to the root that contains this file/dir - so we can deduce permissionts
    // otherwise its an expensive lookup

    return UACStatus::OK;
}


} // namespace tpunkt