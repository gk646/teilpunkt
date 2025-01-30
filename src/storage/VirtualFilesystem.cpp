#include "storage/VirtualFilesystem.h"

namespace tpunkt
{

    VirtualFilesystem::VirtualFilesystem(const FilesystemCreateInfo& info)
        : root(info.name, info.maxSize, nullptr), id(info.id)
    {
    }

    bool VirtualFilesystem::createFile(FileID dir, const FileCreationInfo& info)
    {

    }

    bool VirtualFilesystem::fileExists(FileID file)
    {
    }


    bool VirtualFilesystem::canCreateFile(UserID user, const FileCreationInfo& info)
    {

    }
} // namespace tpunkt