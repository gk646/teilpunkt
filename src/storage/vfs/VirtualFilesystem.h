// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include "fwd.h"
#include "storage/vfs/VirtualDirectory.h"
#include "storage/vfs/VirtualFilesystemCache.h"

namespace tpunkt
{

// File names must be unique per directory - case-sensitive
// Not synced == NOT threadsafe
struct VirtualFilesystem
{
    //===== Lookup =====//

    // Full lookup
    VirtualFile* findFile(FileID file);
    VirtualDirectory* getDir(FileID dir);

    bool dirCanHoldSize(FileID dir, uint64_t additional);

    // Returns the directory this file is in
    VirtualDirectory* getFileDir(FileID file);

    VirtualDirectory& getRoot();


  private:
    explicit VirtualFilesystem(const DirectoryCreationInfo& info);
    TPUNKT_MACROS_MOVE_ONLY(VirtualFilesystem);
    ~VirtualFilesystem();

    VirtualDirectory root;
    std::vector<VirtualDirectory*> iterationCache;
    friend StorageEndpoint;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILESYSTEM_H
