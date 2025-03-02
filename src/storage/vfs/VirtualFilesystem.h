// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include <deque>
#include "storage/vfs/VirtualDirectory.h"
#include "storage/vfs/VirtualFile.h"
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

    //===== Reader =====//

    // True if permitted
    bool fileAddReader(FileID file);
    bool fileRemoveReader(FileID file);
    bool fileAddWriter(FileID file);
    bool fileRemoveWriter(FileID file);

  private:
    std::deque<BlockNode<VirtualDirectory>*> dirCache;
    BlockNode<VirtualDirectory>* root = nullptr;
    VirtualFilesystemCache cache;
    Spinlock systemLock;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILESYSTEM_H