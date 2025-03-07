// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{

#define DIR_ACCESS                                                                                                     \
    stats.accessCount++;                                                                                               \
    stats.lastAccess = Timestamp::Now();

#define DIR_CHANGE                                                                                                     \
    stats.changeCount++;                                                                                               \
    stats.lastEdit = Timestamp::Now();


VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info)
    : info(info.name, info.parent, false), limits(info.maxSize, false)
{
}

VirtualFile* VirtualDirectory::searchFile(const FileID file)
{
    DIR_ACCESS;
    if(file.isDirectory()) [[unlikely]]
    {
        return nullptr;
    }
    return files.get(file.getBlock());
}

VirtualDirectory* VirtualDirectory::searchDir(const FileID dir)
{
    DIR_ACCESS
    if(dir.isFile()) [[unlikely]]
    {
        return nullptr;
    }

    return dirs.get(dir.getBlock());
}

bool VirtualDirectory::fileAdd(const FileCreationInfo& info, uint32_t& idx)
{
    DIR_ACCESS
    if(fileExists(info.name)) [[unlikely]]
    {
        return false;
    }
    DIR_CHANGE;
    files.add(idx, info);
    return true;
}

bool VirtualDirectory::fileRemove(const FileID file)
{
    DIR_ACCESS
    if(file.isDirectory()) [[unlikely]]
    {
        LOG_ERROR("Wrong call");
        return false;
    }
    const auto result = files.remove(file.getBlock());
    if(result)
    {
        DIR_CHANGE;
    }
    return result;
}

bool VirtualDirectory::dirAdd(const DirectoryCreationInfo& info, uint32_t& idx)
{
    DIR_ACCESS
    if(dirExists(info.name)) [[unlikely]]
    {
        return false;
    }
    DIR_CHANGE;
    dirs.add(idx, info);
    return true;
}

bool VirtualDirectory::dirRemove(const FileID dir)
{
    DIR_ACCESS
    if(dir.isFile())
    {
        LOG_ERROR("Wrong call");
        return false;
    }
    const auto result = dirs.remove(dir.getBlock());
    if(result)
    {
        DIR_CHANGE;
    }
    return result;
}

bool VirtualDirectory::fileRemoveAll()
{
    DIR_ACCESS
    const auto result = files.removeAll();
    if(result)
    {
        DIR_CHANGE;
    }
    return result;
}

bool VirtualDirectory::canFit(const uint64_t fileSize) const
{
    const VirtualDirectory* current = this;
    while(current != nullptr)
    {
        if(current->stats.sizeCurrent + fileSize > current->limits.sizeLimit)
        {
            return false;
        }
        current = current->info.parent;
    }
    return true;
}

void VirtualDirectory::propagateChange(const DirFunc func) const
{
    VirtualDirectory* current = info.parent;
    while(current != nullptr)
    {
        func(*current);
        current = current->info.parent;
    }
}

} // namespace tpunkt