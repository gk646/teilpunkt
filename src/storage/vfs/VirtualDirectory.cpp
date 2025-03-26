// SPDX-License-Identifier: GPL-3.0-only

#include "storage/Storage.h"
#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{

VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info, const EndpointID endpoint)
    : info(info.name, info.parent, false, FileID{GetStorage().getNextID(), endpoint, true}), limits(info.maxSize, false)
{
}

VirtualFile* VirtualDirectory::searchFile(const FileID file)
{
    SpinlockGuard guard{lock};
    onAccess();
    if(file.isDirectory()) [[unlikely]]
    {
        return nullptr;
    }

    return getFile(file);
}

VirtualDirectory* VirtualDirectory::searchDir(const FileID dir)
{
    SpinlockGuard guard{lock};
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

bool VirtualDirectory::fileAdd(const FileCreationInfo& info, FileID& file)
{
    SpinlockGuard guard{lock};
    onAccess();
    for(const auto& savedFile : files)
    {
        if(savedFile.getInfo().name == info.name)
        {
            return false;
        }
    }
    const auto& newFile = files.emplace_front(info, this->info.id.getEndpoint());
    stats.fileCount++;
    file = newFile.getID();
    onChange();
    return true;
}

bool VirtualDirectory::fileChange(const FileID file, const uint64_t newFileSize)
{
    SpinlockGuard guard{lock};
    onAccess();

    VirtualFile* changedFile = getFile(file);
    if(changedFile == nullptr) [[unlikely]]
    {
        return false;
    }

    const uint64_t currFileSize = changedFile->getStats().size;
    if(currFileSize == newFileSize) [[unlikely]]
    {
        return false;
    }

    if(!canHoldSizeChange(currFileSize, newFileSize)) [[unlikely]]
    {
        return false;
    }

    bool fitsIntoAll = true;
    VirtualDirectory* current = info.parent;

    // Iterate up with locking
    while(current != nullptr)
    {
        current->lock.lock();
        if(!current->canHoldSizeChange(currFileSize, newFileSize)) [[unlikely]]
        {
            fitsIntoAll = false;
        }
    }

    // Iterate up again to unlock
    current = info.parent;
    while(current != nullptr)
    {
        if(fitsIntoAll)
        {
            current->stats.subDirFileSize = current->stats.subDirFileSize - currFileSize + newFileSize;
            current->onChange();
        }
        current->lock.unlock();
        current = current->info.parent;
    }

    // Return early
    if(!fitsIntoAll)
    {
        return false;
    }

    stats.fileSize = stats.fileSize - currFileSize + newFileSize;
    changedFile->stats.size = newFileSize;
    changedFile->onChange();
    onChange();
    return true;
}

bool VirtualDirectory::fileExists(const FileName& name) const
{
    SpinlockGuard guard{lock};
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

bool VirtualDirectory::fileRemove(const FileID file)
{
    SpinlockGuard guard{lock};
    onAccess();
    if(file.isDirectory()) [[unlikely]]
    {
        LOG_ERROR("Wrong call");
        return false;
    }
    const auto result = files.remove_if([ & ](const VirtualFile& checked) { return checked.getInfo().id == file; }) > 0;
    if(result)
    {
        onChange();
    }
    return result;
}

bool VirtualDirectory::fileRemoveAll()
{
    SpinlockGuard guard{lock};
    onAccess();
    if(stats.fileCount > 0)
    {
        files.clear();
        onChange();
        return true;
    }
    return false;
}

//===== Directories =====//

bool VirtualDirectory::dirAdd(const DirectoryCreationInfo& info)
{
    SpinlockGuard guard{lock};
    onAccess();
    for(auto& savedDir : dirs)
    {
        if(savedDir.info.name == info.name) [[unlikely]]
        {
            return false;
        }
    }
    onChange();
    stats.dirCount++;
    dirs.emplace_front(info, this->info.id.getEndpoint());
    return true;
}

bool VirtualDirectory::dirRemove(const FileID dir)
{
    SpinlockGuard guard{lock};
    onAccess();
    if(dir.isFile())
    {
        LOG_ERROR("Wrong call");
        return false;
    }
    const auto result = dirs.remove_if([ & ](const VirtualDirectory& checked) { return checked.info.id == dir; }) > 0;
    if(result)
    {
        onChange();
    }
    return result;
}

bool VirtualDirectory::dirExists(const FileName& name) const
{
    SpinlockGuard guard{lock};
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

std::forward_list<VirtualFile, SharedBlockAllocator<VirtualFile>>& VirtualDirectory::getFiles()
{
    return files;
}

void VirtualDirectory::rename(const FileName& name)
{
    SpinlockGuard guard{lock};
    onAccess();
    onChange();
    info.name = name;
}

const DirectoryStats& VirtualDirectory::getStats() const
{
    onAccess();
    return stats;
}

const DirectoryLimits& VirtualDirectory::getLimits() const
{
    onAccess();
    return limits;
}

//===== Private =====//

// Locking is assumed for those functions

VirtualFile* VirtualDirectory::getFile(const FileID file)
{
    for(auto& savedFile : files)
    {
        if(savedFile.getID() == file)
        {
            return &savedFile;
        }
    }
    return nullptr;
}

bool VirtualDirectory::canHoldSizeChange(const uint64_t currSize, const uint64_t newSize) const
{
    return (stats.fileSize + stats.subDirFileSize - currSize + newSize) < limits.sizeLimit;
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

template <typename Func>
void VirtualDirectory::iterateParents(const Func func) const
{
    VirtualDirectory* current = info.parent;

    // Iterate up with locking
    while(current != nullptr)
    {
        current->lock.lock();
        const auto changed = func(*current);
        current->onAccess();
        if(changed)
        {
            current->onChange();
        }
        current = current->info.parent;
    }

    // Iterate up to unlock
    current = info.parent;
    while(current != nullptr)
    {
        current->lock.unlock();
        current = current->info.parent;
    }
}


} // namespace tpunkt