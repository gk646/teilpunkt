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

int64_t VirtualFile::changeSize(const uint64_t newSize)
{
    SpinlockGuard guard{lock};
    onAccess();
    const int64_t currentSize = static_cast<int64_t>(stats.size);
    const int64_t diff = static_cast<int64_t>(newSize) - currentSize;
    // Must be >0
    stats.size = static_cast<uint64_t>(currentSize + diff);
    onChange();
    return diff;
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