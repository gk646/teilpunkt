#include "storage/VirtualFilesystem.h"

namespace tpunkt
{

    VirtualFilesystem::VirtualFilesystem(const FilesystemCreateInfo& info)
        : root(DirectoryCreationInfo{.name = info.name,
                                     .maxSize = info.maxSize,
                                     .parent = nullptr,
                                     .id = info.rootID,
                                     .creator = info.creator}),
          id(info.endpointID)
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