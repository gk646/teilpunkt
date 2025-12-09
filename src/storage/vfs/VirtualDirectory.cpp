// SPDX-License-Identifier: GPL-3.0-only

#include <algorithm>
#include "storage/Storage.h"
#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{

VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info, const EndpointID endpoint)
    : info(info.name, info.parent, false, FileID{Storage::GetInstance().getNextID(), endpoint, true}),
      limits(info.maxSize, false)
{
}

VirtualFile* VirtualDirectory::findFile(const FileID file)
{
    SpinlockGuard guard{lock};
    for(auto& child : files)
    {
        if(child.getID() == file)
        {
            return &child;
        }
    }
    return nullptr;
}

VirtualDirectory* VirtualDirectory::findDir(const FileID dir)
{
    SpinlockGuard guard{lock};
    for(auto& subdir : dirs)
    {
        if(subdir.getID() == dir)
        {
            return &subdir;
        }
    }
    return nullptr;
}

bool VirtualDirectory::fileAdd(const FileCreationInfo& createInfo, FileID& file)
{
    SpinlockGuard guard{lock};
    for(const auto& savedFile : files)
    {
        if(savedFile.getInfo().name == createInfo.name)
        {
            return false;
        }
    }
    onModification();
    auto& newFile = files.emplace_back(createInfo);
    file = newFile.getID();
    return true;
}

bool VirtualDirectory::fileChange(const FileID file, const uint64_t newFileSize)
{
    SpinlockGuard guard{lock};

    VirtualFile* changeFile = findFile(file);
    if(changeFile == nullptr) [[unlikely]]
    {
        return false;
    }

    const uint64_t currFileSize = changeFile->getStats().size;
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
        current = current->info.parent;
    }

    // Iterate up again to unlock
    current = info.parent;
    while(current != nullptr)
    {
        if(fitsIntoAll)
        {
            current->stats.subDirFileSize = (current->stats.subDirFileSize - currFileSize) + newFileSize;
            current->onModification();
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
    changeFile->stats.size = newFileSize;
    changeFile->onModification();
    onModification();
    return true;
}

bool VirtualDirectory::fileExists(const FileName& name) const
{
    SpinlockGuard guard{lock};
    for(const auto& file : files)
    {
        if(file.getInfo().name == name)
        {
            return true;
        }
    }
    return false;
}

// Has no locking
bool VirtualDirectory::fileDeleteImpl(FileID file)
{
    auto* changeFile = findFile(file);
    if(changeFile == nullptr) [[unlikely]]
    {
        return false;
    }

    const FileStats changeFileStats = changeFile->getStats();
    if(std::erase_if(files, [ file ](const VirtualFile& f) { return f.getID() == file; }) > 0)
    {
        onModification();
        stats.fileSize -= changeFileStats.size;
        auto changeFunc = [ & ](VirtualDirectory& dir)
        {
            dir.stats.subDirFileSize -= changeFileStats.size;
            return true;
        };
        iterateParents(changeFunc);
        return true;
    }
    return false;
}

bool VirtualDirectory::fileDelete(const FileID file)
{
    SpinlockGuard guard{lock};
    return fileDeleteImpl(file);
}

bool VirtualDirectory::fileDeleteAll()
{
    SpinlockGuard guard{lock};
    if(!files.empty()) [[likely]]
    {
        for(auto& file : files)
        {
            fileDeleteImpl(file.getID());
        }
        return true;
    }
    return false;
}

//===== Directories =====//

bool VirtualDirectory::dirAdd(const DirectoryCreationInfo& info, FileID& dir)
{
    SpinlockGuard guard{lock};
    if(info.maxSize >= limits.sizeLimit) [[unlikely]]
    {
        return false;
    }

    for(auto& savedDir : dirs)
    {
        if(savedDir.info.name == info.name) [[unlikely]]
        {
            return false;
        }
    }
    onModification();
    const auto& newDir = dirs.emplace_front(info, this->info.id.getEndpoint());
    dir = newDir.getID();
    return true;
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

bool VirtualDirectory::dirDelete(const FileID dir)
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
        onModification();
    }
    return result;
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
    onModification();
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

FileID VirtualDirectory::getID() const
{
    return id;
}

//===== Private =====//

// Locking is assumed for those functions

bool VirtualDirectory::canHoldSizeChange(const uint64_t currSize, const uint64_t newSize) const
{
    return (stats.fileSize + stats.subDirFileSize - currSize + newSize) < limits.sizeLimit;
}

void VirtualDirectory::onAccess()
{
    stats.accessCount++;
    stats.lastAccess = Timestamp::Now();
}

void VirtualDirectory::onModification()
{
    stats.changeCount++;
    stats.lastEdit = Timestamp::Now();
}


} // namespace tpunkt
