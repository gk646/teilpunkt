// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{

VirtualFile::VirtualFile(const FileCreationInfo& info)
    : file(Storage::GetInstance().getNextID()), info(info.name, info.creator), stats(Timestamp::Now())
{
}

VirtualFile::VirtualFile(VirtualFile&& other) noexcept
    : file(other.file), info(other.info), stats(other.stats), history(other.history)
{
}

VirtualFile& VirtualFile::operator=(VirtualFile&& other) noexcept
{
    SpinlockGuard guard{lock};
    SpinlockGuard oGuard{other.lock};

    if(this != &other)
    {
        file = other.file;
        info = other.info;
        stats = other.stats;
        history = other.history;
    }
    return *this;
}

void VirtualFile::rename(const FileName& newName)
{
    SpinlockGuard guard{lock};
    info.name = newName;
    onModification();
}

const FileInfo& VirtualFile::getInfo() const
{
    SpinlockGuard guard{lock};
    return info;
}

const FileStats& VirtualFile::getStats() const
{
    SpinlockGuard guard{lock};
    return stats;
}

FileID VirtualFile::getID() const
{
    return file;
}

void VirtualFile::onAccess()
{
    SpinlockGuard guard{lock};
    stats.accessed = Timestamp::Now();
    stats.accessCount++;
}

void VirtualFile::onModification()
{
    SpinlockGuard guard{lock};
    stats.modified = Timestamp::Now();
    stats.modificationCount++;
}

} // namespace tpunkt
