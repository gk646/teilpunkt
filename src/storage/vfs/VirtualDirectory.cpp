// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{

#define DIR_ACCESS                                                                                                     \
    CooperativeSpinlockGuard guard{lock, false};                                                                       \
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
    for(auto& savedFile : files)
    {
        return &savedFile;
    }
    return nullptr;
}

VirtualDirectory* VirtualDirectory::searchDir(const FileID dir)
{
    DIR_ACCESS
    if(dir.isFile()) [[unlikely]]
    {
        return nullptr;
    }

    for(auto& savedDir : dirs)
    {
        return &savedDir;
    }
    return nullptr;
}

bool VirtualDirectory::fileAdd(const FileCreationInfo& info)
{
    {
        DIR_ACCESS
        if(fileExists(info.name)) [[unlikely]]
        {
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    DIR_CHANGE;
    stats.fileCount++;
    files.emplace_front(info);
    return true;
}


bool VirtualDirectory::fileRemove(const FileID file)
{
    {
        DIR_ACCESS
        if(file.isDirectory()) [[unlikely]]
        {
            LOG_ERROR("Wrong call");
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    const auto result = files.remove_if([ & ](const VirtualFile& checked) { return checked.info.id == file; }) > 0;
    if(result)
    {
        DIR_CHANGE
    }
    return result;
}

bool VirtualDirectory::dirAdd(const DirectoryCreationInfo& info)
{
    {
        DIR_ACCESS
        if(dirExists(info.name)) [[unlikely]]
        {
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    DIR_CHANGE;
    stats.dirCount++;
    dirs.emplace_front(info);
    return true;
}

bool VirtualDirectory::dirRemove(const FileID dir)
{
    {
        DIR_ACCESS
        if(dir.isFile())
        {
            LOG_ERROR("Wrong call");
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    const auto result = files.remove_if([ & ](const VirtualFile& checked) { return checked.info.id == dir; }) > 0;
    if(result)
    {
        DIR_CHANGE
    }
    return result;
}

bool VirtualDirectory::fileRemoveAll()
{
    bool changed = false;
    {
        DIR_ACCESS
        changed = stats.fileCount > 0;
    }
    if(changed)
    {
        CooperativeSpinlockGuard guard{lock, true};
        files.clear();
        DIR_CHANGE;
    }
    return changed;
}

bool VirtualDirectory::canFit(const uint64_t fileSize)
{
    DIR_ACCESS
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