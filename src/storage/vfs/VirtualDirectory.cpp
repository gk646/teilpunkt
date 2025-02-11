// SPDX-License-Identifier: Apache License 2.0

#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{
VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info)
    : info(info.name, info.maxSize, 0U, info.id, info.parent, info.creator)
{
}

VirtualDirectory* VirtualDirectory::searchDir(const FileID dirid)
{
    for(auto& dir : subdirectories)
    {
        if(dir.info.id == dirid)
        {
            return &dir;
        }
    }
    return nullptr;
}

bool VirtualDirectory::addFile(const FileCreationInfo& info, const bool fits, const bool unique)
{
    if(fits && unique)
    {
        files.emplace_back(info);
        propagateAddFile(info.size);
        return true;
    }
    return false;
}

bool VirtualDirectory::removeFile(const FileID fileid)
{
    if(fileid.isDirectory)
    {
        return false;
    }

    for(auto& file : files)
    {
        if(file.info.id == fileid)
        {
            GetEventMonitor().logData<EventType::FileSystem>(EventAction::FilesystemRemoveFile, EventStatus::SUCCESS,
                                                             FileSystemEventData{file.info.id});
            propagateRemoveFile(file.info.size);
            file = std::move(files.back());
            files.pop_back();
            return true;
        }
    }
    return false;
}

bool VirtualDirectory::addDirectory(const DirectoryCreationInfo& info)
{
    propagateAddDir();
    subdirectories.emplace_back(info);
    return true;
}

bool VirtualDirectory::removeDirectory(const FileID dirid)
{
    if(!dirid.isDirectory)
    {
        return false;
    }

    for(auto& dir : subdirectories)
    {
        if(dir.info.id == dirid)
        {
            if(dir.getFileCount() != 0)
            {
                return false; // Cant remove
            }
            // TODO proper move
            propagateRemoveDir();
            dir = std::move(subdirectories.back());
            subdirectories.pop_back();
            return true;
        }
    }
    return false;
}

bool VirtualDirectory::removeAllFiles()
{
    for(int i = 0; i < files.size(); ++i)
    {
        removeFile(files[ i ].info.id);
    }
    return true;
}

bool VirtualDirectory::canFit(const uint64_t fileSize) const
{
    const VirtualDirectory* current = this;
    while(current != nullptr)
    {
        if(current->info.sizeCurrent + fileSize > current->info.sizeLimit)
        {
            return false;
        }
        current = current->info.parent;
    }
    return true;
}

bool VirtualDirectory::nameExists(const FileName& name) const
{
    for(const auto& file : files)
    {
        if(file.info.name == name)
        {
            return true;
        }
    }
    for(const auto& dir : subdirectories)
    {
        if(dir.info.name == name)
        {
            return true;
        }
    }
    return false;
}

uint32_t VirtualDirectory::getFileCount() const
{
    return files.size();
}

uint32_t VirtualDirectory::getDirCount() const
{
    return subdirectories.size();
}

uint32_t VirtualDirectory::getTotalFileCount() const
{
    return getFileCount() + stats.subdirFileCount;
}

uint32_t VirtualDirectory::getTotalDirCount() const
{
    return getDirCount() + stats.subdirDirCount;
}

void VirtualDirectory::propagateAddFile(const uint64_t size)
{
    VirtualDirectory* current = this;
    while(current != nullptr)
    {
        if(current->info.sizeCurrent + size > current->info.sizeLimit)
        {
            LOG_CRITICAL("Internal Error: Directory Size Mismatch");
            return;
        }
        current->info.sizeCurrent += size;
        if(current != this) [[likely]]
        {
            current->stats.subdirFileCount++;
        }
        current = current->info.parent;
    }
}

void VirtualDirectory::propagateRemoveFile(const uint64_t size)
{
    VirtualDirectory* current = this;
    while(current != nullptr)
    {
        if(current->info.sizeCurrent < size) [[unlikely]]
        {
            LOG_CRITICAL("Internal Error: Directory Size Mismatch");
            return;
        }
        current->info.sizeCurrent -= size;
        if(current != this) [[likely]]
        {
            if(current->stats.subdirFileCount == 0)
            {
                LOG_CRITICAL("Internal Error: Subdir File Count Mismatch");
                return;
            }
            current->stats.subdirFileCount--;
        }
        current = current->info.parent;
    }
}

void VirtualDirectory::propagateAddDir() const
{
    VirtualDirectory* current = this->info.parent;
    while(current != nullptr)
    {
        current->stats.subdirDirCount++;
        current = current->info.parent;
    }
}

void VirtualDirectory::propagateRemoveDir() const
{
    VirtualDirectory* current = this->info.parent;
    while(current != nullptr)
    {
        if(current->stats.subdirFileCount == 0)
        {
            LOG_CRITICAL("Internal Error: Subdir Dir Count Mismatch");
            return;
        }
        current->stats.subdirDirCount--;
        current = current->info.parent;
    }
}

} // namespace tpunkt