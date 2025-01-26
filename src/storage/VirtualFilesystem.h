#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include "datastructures/FixedString.h"
#include "storage/VirtualFile.h"


// Uses "/" as separator

namespace tpunkt
{
    struct FileDescriptor final
    {
        static FileDescriptor fromString(const char* string);

      private:
        bool valid = false;
        FileSystemPath path;
    };

    struct FilesystemCreateInfo final
    {
        FixedString<50> name;
        uint64_t maxSize; // Max size in bytes
    };

    struct VirtualFilesystem
    {
        VirtualFilesystem(const FilesystemCreateInfo& info);

        bool fileExists(const FileDescriptor& name);

        bool canCreateFile(UserID user, const FileCreationInfo& info);

      private:
        VirtualDirectory root;
    };

} // namespace tpunkt


#endif // TPUNKT_VIRTUAL_FILESYSTEM_H