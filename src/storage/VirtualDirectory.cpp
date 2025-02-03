#include "storage/VirtualDirectory.h"

namespace tpunkt
{
    VirtualDirectory::VirtualDirectory(const DirectoryCreationInfo& info)
        : name(info.name), parent(info.parent), sizeLimit(info.maxSize)
    {
    }

    bool VirtualDirectory::fileAdd(const FileCreationInfo& info, FileID file, const bool fits, const bool unique)
    {
        if(fits && unique)
        {
            files.emplace_back(info, file);
            propagateSizeChange(info.size, false);
            return true;
        }
        return false;
    }


    bool VirtualDirectory::canFit(const uint64_t fileSize) const
    {
        const VirtualDirectory* current = this;
        while(current != nullptr)
        {
            if(current->sizeCurrent + fileSize >= current->sizeLimit)
            {
                return false;
            }
            current = current->parent;
        }
        return true;
    }


    void VirtualDirectory::propagateSizeChange(const uint64_t change, const bool subtract)
    {
        VirtualDirectory* current = this;
        while(current != nullptr)
        {
            if(subtract)
            {
                if(current->sizeCurrent < change)
                {
                    LOG_CRITICAL("Internal Error: Directory Size Mismatch");
                    return;
                }
                current->sizeCurrent -= change;
            }
            else
            {
                if(current->sizeCurrent + change >= current->sizeLimit)
                {
                    LOG_CRITICAL("Internal Error: Directory Size Mismatch");
                    return;
                }
                current->sizeCurrent += change;
            }
            current = current->parent;
        }
    }

} // namespace tpunkt