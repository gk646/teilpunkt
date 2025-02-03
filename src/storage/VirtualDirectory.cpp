#include "storage/VirtualDirectory.h"

namespace tpunkt
{
    VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info)
        : info(info.name, info.maxSize, 0U, info.id, info.parent, info.creator)
    {
    }

    bool VirtualDirectory::fileAdd(const FileCreationInfo& info, const bool fits, const bool unique)
    {
        if(fits && unique)
        {
            files.emplace_back(info);
            propagateAdd(info.size);
            return true;
        }
        return false;
    }

    bool VirtualDirectory::fileRemove(const FileID fileID)
    {
        if(fileID.isDirectory)
        {
            return false;
        }

        for(auto& file : files)
        {
            if(file.info.id == fileID)
            {
                propagateRemove(file.info.size);
                file = std::move(files.back());
                files.pop_back();
                return true;
            }
        }
        return false;
    }

    bool VirtualDirectory::directoryAdd(const DirectoryCreationInfo& info)
    {
        VirtualDirectory dir{info};
        subdirectories.emplace_back(info);
        return true;
    }

    bool VirtualDirectory::directoryRemove(const FileID fileID)
    {
        if(!fileID.isDirectory)
        {
            return false;
        }

        for(auto& dir : subdirectories)
        {
            if(dir.info.id == fileID)
            {
                if(dir.getFileCount() != 0)
                {
                    return false; // Cant remove
                }
                dir = std::move(subdirectories.back());
                subdirectories.pop_back();
                return true;
            }
        }
        return false;
    }

    bool VirtualDirectory::canFit(const uint64_t fileSize) const
    {
        const VirtualDirectory* current = this;
        while(current != nullptr)
        {
            if(current->info.sizeCurrent + fileSize >= current->info.sizeLimit)
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

    void VirtualDirectory::propagateAdd(const uint64_t size)
    {
        VirtualDirectory* current = this;
        while(current != nullptr)
        {
            if(current->info.sizeCurrent + size >= current->info.sizeLimit)
            {
                LOG_CRITICAL("Internal Error: Directory Size Mismatch");
                return;
            }
            current->info.sizeCurrent += size;
            current = current->info.parent;
        }
    }

    void VirtualDirectory::propagateRemove(const uint64_t size)
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
            current = current->info.parent;
        }
    }

} // namespace tpunkt