#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include "datastructures/FixedString.h"
#include "storage/VirtualFile.h"
#include "storage/FileDescriptors.h"

// Uses "/" as separator

namespace tpunkt
{

    struct FilesystemCreateInfo final
    {
        FileName name;
        uint64_t maxSize; // Max size in bytes
        uint8_t id;
    };

    struct VirtualFilesystem
    {
        explicit VirtualFilesystem(const FilesystemCreateInfo& info);

        bool createFile(FileID dir, const FileCreationInfo& info);

        bool fileExists(FileID file);

        bool canCreateFile(UserID user, const FileCreationInfo& info);

      private:
        VirtualDirectory root;
        uint8_t id;
    };

} // namespace tpunkt


#endif // TPUNKT_VIRTUAL_FILESYSTEM_H