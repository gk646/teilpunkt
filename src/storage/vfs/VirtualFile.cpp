// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{

VirtualFile::VirtualFile(const FileCreationInfo& info)
    : file(Storage::GetInstance().getNextID(), false), info(info.name, info.creator), stats(Timestamp::Now())
{
}

void VirtualFile::rename(const FileName& newName)
{
    info.name = newName;
    onModification();
}

const FileInfo& VirtualFile::getInfo() const
{
    return info;
}

const FileStats& VirtualFile::getStats() const
{
    return stats;
}

FileID VirtualFile::getID() const
{
    return file;
}

void VirtualFile::onAccess()
{
    stats.accessed = Timestamp::Now();
    stats.accessCount++;
}

void VirtualFile::onModification()
{
    onAccess();
    stats.modified = Timestamp::Now();
    stats.modificationCount++;
}

} // namespace tpunkt
