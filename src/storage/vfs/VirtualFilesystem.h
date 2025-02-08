#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include "storage/vfs/VirtualDirectory.h"
#include "storage/vfs/VirtualFilesystemCache.h"
#include "datastructures/FixedString.h"
#include "storage/vfs/VirtualFile.h"
#include "datastructures/Spinlock.h"


namespace tpunkt
{

    struct FilesystemCreateInfo final
    {
        FileName name;
        uint64_t maxSize = 0; // Max size in bytes
        FileID rootID;
        uint8_t endpointID = 0;
        UserID creator{};
    };

    // File names must be unique per directory - case-sensitive
    // Any modifications are atomic for the whole filesystem - for simplicity
    struct VirtualFilesystem
    {
        explicit VirtualFilesystem(const FilesystemCreateInfo& info);

        bool createFile(FileID dir, const FileCreationInfo& info);

        bool fileExists(FileID file);

        bool canCreateFile(UserID user, const FileCreationInfo& info);

      private:
        VirtualDirectory root;
        VirtualFilesystemCache cache;
        Spinlock systemLock;
        uint8_t id;
    };

} // namespace tpunkt


#endif // TPUNKT_VIRTUAL_FILESYSTEM_H