// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include "datastructures/FixedString.h"
#include "datastructures/Spinlock.h"
#include "storage/vfs/VirtualDirectory.h"
#include "storage/vfs/VirtualFile.h"
#include "storage/vfs/VirtualFilesystemCache.h"

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
struct VirtualFilesystem
{
    explicit VirtualFilesystem(const FilesystemCreateInfo& info);
    TPUNKT_MACROS_MOVE_ONLY(VirtualFilesystem);

    ~VirtualFilesystem();

    //===== File Basic =====//

    // Full lookup
    VirtualFile* fileGet(FileID file);

    // Only searches in given dir
    VirtualFile* fileGetInDir(FileID file, FileID dir);

    bool fileCreate(User user, FileID dir, const FileCreationInfo& info, FileID file);

    bool fileDelete(User user, FileID dir);

    //===== File Action =====//

    // True if permitted
    bool fileAddReader(UserID user);

    bool fileRemoveReader(UserID user);

    VirtualDirectory* dirGet();

    bool dirDelete();

    bool dirCreate();

  private:
    VirtualDirectory root;
    VirtualFilesystemCache cache;
    Spinlock systemLock;
    uint8_t id;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILESYSTEM_H