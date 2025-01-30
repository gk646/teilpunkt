#include "storage/VirtualFile.h"

#include "Storage.h"

namespace tpunkt
{
    //===== Virtual File =====//

    VirtualFile::VirtualFile(const FileCreationInfo& info, const FileID file)
        : info(info.name, info.creator, info.size, file)
    {
    }

    //===== Virtual Directory =====//

    VirtualDirectory::VirtualDirectory(const FileName& name, const uint64_t maxSize, VirtualDirectory* parent)
        : name(name), parent(parent), sizeLimit(maxSize)
    {
    }

    bool VirtualDirectory::fileAdd(const FileCreationInfo& info, FileID file)
    {
        if(fileFits(info))
        {
            propagateSizeChange(info.size, false);
            files.emplace_back(info, file);
            return true;
        }
        return false;
    }

    bool VirtualDirectory::fileRemove(FileID fileID)
    {
        for(const auto& file : files)
        {
            if(file.info.id == fileID)
            {

            }
        }
    }

    bool VirtualDirectory::fileFits(const FileCreationInfo& info) const
    {
        return (sizeCurrent + info.size) < sizeLimit;
    }

    void VirtualDirectory::propagateSizeChange(const uint64_t change, const bool subtract)
    {
        VirtualDirectory* current = this;
        while(current != nullptr)
        {
            if(subtract)
            {
                current->sizeCurrent -= change;
            }
            else
            {
                current->sizeCurrent += change;
            }
            current = current->parent;
        }
    }

} // namespace tpunkt