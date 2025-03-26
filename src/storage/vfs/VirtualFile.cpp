// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{

VirtualFile::VirtualFile(const FileCreationInfo& info, const EndpointID endpoint)
    : info(info.name, info.creator, FileID{GetStorage().getNextID(), endpoint, false})
{
}

const FileInfo& VirtualFile::getInfo() const
{
    SpinlockGuard guard{lock};
    onAccess();
    return info;
}

const FileStats& VirtualFile::getStats() const
{
    SpinlockGuard guard{lock};
    onAccess();
    return stats;
}

FileID VirtualFile::getID() const
{
    return getInfo().id;
}

void VirtualFile::onAccess() const
{
}

void VirtualFile::onChange()
{
}

} // namespace tpunkt