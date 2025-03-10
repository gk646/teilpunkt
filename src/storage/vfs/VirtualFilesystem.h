// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include <deque>
#include "storage/vfs/VirtualDirectory.h"
#include "storage/vfs/VirtualFilesystemCache.h"

namespace tpunkt
{

// File names must be unique per directory - case-sensitive
struct VirtualFilesystem
{
    explicit VirtualFilesystem(const DirectoryCreationInfo& info);
    TPUNKT_MACROS_MOVE_ONLY(VirtualFilesystem);
    ~VirtualFilesystem();

    //===== Lookup =====//

    // Full lookup
    VirtualFile* getFile(FileID file);
    VirtualDirectory* getDir(FileID dir);

    // Returns the directory this file is in
    VirtualDirectory* getFileDir(FileID file);

  private:
    std::deque<VirtualDirectory*> dirCache;
    VirtualDirectory root;
    Spinlock systemLock;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILESYSTEM_H