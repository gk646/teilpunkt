// SPDX-License-Identifier: GPL-3.0-only

#include <algorithm>
#include "storage/Storage.h"
#include "storage/vfs/VirtualDirectory.h"

namespace tpunkt
{

VirtualDirectory::VirtualDirectory(const DirCreationInfo& info)
    : fid(info.parent.getEndpoint(), true),
      info(FileInfo{.name = info.name, .creator = info.creator, .owner = info.creator}, info.parent),
      limits(info.maxSize, false)
{
}

VirtualFile* VirtualDirectory::findFile(const FileID file)
{
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
    if(fileNameExists(createInfo.name))
    {
        return false;
    }
    onModification();
    auto& newFile = files.emplace_back(createInfo);
    file = newFile.getID();
    return true;
}

bool VirtualDirectory::fileChangeSize(const FileID file, const uint64_t newFileSize)
{
    VirtualFile* changeFile = findFile(file);
    if(changeFile == nullptr) [[unlikely]]
    {
        return false;
    }

    const uint64_t currFileSize = changeFile->getStats().size;
    if(currFileSize == newFileSize) [[unlikely]]
    {
        return true;
    }

    if(!canHoldSizeChange(currFileSize, newFileSize)) [[unlikely]]
    {
        return false;
    }

    stats.base.size = stats.base.size - currFileSize + newFileSize;
    changeFile->stats.size = newFileSize;
    changeFile->onModification();
    onModification();
    return true;
}

bool VirtualDirectory::fileDelete(const FileID file)
{
    return fileDeleteImpl(file);
}

bool VirtualDirectory::fileDeleteAll()
{
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

bool VirtualDirectory::fileNameExists(const FileName& name) const
{
    return std::ranges::any_of(files, [ & ](auto& dir) { return dir.info.name == name; });
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
    if(std::erase_if(files, [ file ](const VirtualFile& f) -> bool { return f.getID() == file; }) > 0)
    {
        onModification();
        stats.base.size -= changeFileStats.size;
        auto changeFunc = [ & ](VirtualDirectory& dir) -> bool
        {
            dir.stats.subDirFileSize -= changeFileStats.size;
            return true;
        };
        // iterateParents(changeFunc);
        return true;
    }
    return false;
}

//===== Directories =====//

bool VirtualDirectory::dirAdd(const DirCreationInfo& cInfo, FileID& dir)
{
    if(dirNameExists(cInfo.name)) [[unlikely]]
    {
        return false;
    }

    onModification();
    auto& newDirRef = dirs.emplace_back(cInfo);
    dir = newDirRef.getID();

    return true;
}

bool VirtualDirectory::dirNameExists(const FileName& name) const
{
    return std::ranges::any_of(dirs, [ & ](auto& dir) { return dir.info.base.name == name; });
}

bool VirtualDirectory::dirDelete(const FileID dir)
{
    onAccess();
    if(!dir.isDirectory())
    {
        LOG_WARNING("Trying to delete directory ");
        return false;
    }

    const auto result = std::erase_if(dirs, [ & ](const VirtualDirectory& e) { return e.fid == dir; }) > 0;
    if(result)
    {
        onModification();
    }
    return result;
}

void VirtualDirectory::rename(const FileName& name)
{
    onModification();
    info.base.name = name;
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
    return fid;
}

std::vector<VirtualFile>& VirtualDirectory::getFiles()
{
    return files;
}
std::vector<VirtualDirectory>& VirtualDirectory::getDirs()
{
    return dirs;
}

void VirtualDirectory::collectEntries(std::vector<DTO::DirectoryEntry>& entries) const
{
    entries.clear();
    entries.reserve(files.size() + dirs.size() + 1);

    for(const auto& file : files)
    {
        entries.push_back(DTO::DirectoryEntry::FromFile(file));
    }
    for(const auto& dir : dirs)
    {
        entries.push_back(DTO::DirectoryEntry::FromDir(dir));
    }
}

bool VirtualDirectory::canHoldSizeChange(const uint64_t currSize, const uint64_t newSize) const
{
    return (stats.base.size + stats.subDirFileSize - currSize + newSize) < limits.sizeLimit;
}

void VirtualDirectory::onAccess()
{
    stats.base.accessCount++;
    stats.base.accessed = Timestamp::Now();
}

void VirtualDirectory::onModification()
{
    onAccess();
    stats.base.modificationCount++;
    stats.base.modified = Timestamp::Now();
}


} // namespace tpunkt
