// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{

VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info)
    : info(info.name, info.parent, false), limits(info.maxSize, false)
{
}

VirtualFile* VirtualDirectory::searchFile(const FileID file)
{
    CooperativeSpinlockGuard guard{lock, false};
    onAccess();
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
    CooperativeSpinlockGuard guard{lock, false};
    onAccess();
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
        CooperativeSpinlockGuard guard{lock, false};
        onAccess();
        if(fileExists(info.name)) [[unlikely]]
        {
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    stats.fileCount++;
    files.emplace_front(info);
    onChange();
    return true;
}

bool VirtualDirectory::fileRemove(const FileID file)
{
    {
        CooperativeSpinlockGuard guard{lock, false};
        onAccess();
        if(file.isDirectory()) [[unlikely]]
        {
            LOG_ERROR("Wrong call");
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    const auto result = files.remove_if([ & ](const VirtualFile& checked) { return checked.getInfo().id == file; }) > 0;
    if(result)
    {
        onChange();
    }
    return result;
}

bool VirtualDirectory::dirAdd(const DirectoryCreationInfo& info)
{
    {
        CooperativeSpinlockGuard guard{lock, false};
        onAccess();
        if(dirExists(info.name)) [[unlikely]]
        {
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    onChange();
    stats.dirCount++;
    dirs.emplace_front(info);
    return true;
}

bool VirtualDirectory::dirRemove(const FileID dir)
{
    {
        CooperativeSpinlockGuard guard{lock, false};
        onAccess();
        if(dir.isFile())
        {
            LOG_ERROR("Wrong call");
            return false;
        }
    }
    CooperativeSpinlockGuard guard{lock, true};
    const auto result = files.remove_if([ & ](const VirtualFile& checked) { return checked.getInfo().id == dir; }) > 0;
    if(result)
    {
        onChange();
    }
    return result;
}

bool VirtualDirectory::dirExists(const FileName& name) const
{
    CooperativeSpinlockGuard guard{lock, false};
    onAccess();
    for(const auto& dir : dirs)
    {
        if(dir.info.name == name)
        {
            return true;
        }
    }
    return false;
}

std::forward_list<VirtualDirectory, SharedBlockAllocator<VirtualDirectory>>& VirtualDirectory::getDirs()
{
    return dirs;
}

bool VirtualDirectory::fileRemoveAll()
{
    bool changed = false;
    {
        CooperativeSpinlockGuard guard{lock, false};
        onAccess();
        changed = stats.fileCount > 0;
    }
    if(changed)
    {
        CooperativeSpinlockGuard guard{lock, true};
        files.clear();
        onChange();
    }
    return changed;
}

bool VirtualDirectory::fileExists(const FileName& name) const
{
    CooperativeSpinlockGuard guard{lock, false};
    onAccess();
    for(const auto& file : files)
    {
        if(file.getInfo().name == name)
        {
            return true;
        }
    }
    return false;
}

bool VirtualDirectory::canFit(const uint64_t fileSize) const
{
    CooperativeSpinlockGuard guard{lock, false};
    onAccess();
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

void VirtualDirectory::rename(const FileName& name)
{
    CooperativeSpinlockGuard guard{lock, true};
    onAccess();
    onChange();
    info.name = name;
}

const DirectoryStats& VirtualDirectory::getStats() const
{
    return stats;
}

const DirectoryLimits& VirtualDirectory::getLimits() const
{
    return limits;
}

void VirtualDirectory::onAccess() const
{
    stats.accessCount++;
    stats.lastAccess = Timestamp::Now();
}

void VirtualDirectory::onChange() const
{
    stats.changeCount++;
    stats.lastEdit = Timestamp::Now();
}

void VirtualDirectory::propagateChange(const DirFunc func) const
{
    VirtualDirectory* current = info.parent;
    while(current != nullptr)
    {
        CooperativeSpinlockGuard guard{current->lock, true};
        const auto changed = func(*current);
        current->onAccess();
        if(changed)
        {
            current->onChange();
        }
        current = current->info.parent;
    }
}

} // namespace tpunkt