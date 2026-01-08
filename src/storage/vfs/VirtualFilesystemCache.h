// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_FILESYSTEM_CACHE_H
#define TPUNKT_VIRTUAL_FILESYSTEM_CACHE_H

#include <list>
#include <ankerl/unordered_dense.h>
#include "common/FileID.h"
#include "fwd.h"

namespace tpunkt
{

// Saves a quick access position of a file or directory
// Cache entry is invalidated on: deletion, move
struct VirtualFilesystemCache final
{
    VirtualFilesystemCache(uint32_t maxEntries);

    VirtualDirectory* searchDir(FileID dir);

    VirtualFile* searchFile(FileID file);

    void setMaxSize(uint32_t size);

  private:
    uint32_t maxEntries;

    struct CacheEntry final
    {
        FileID id;
        VirtualFile* file;
        VirtualDirectory* directory;
    };

    ankerl::unordered_dense::map<FileID, std::list<CacheEntry>::iterator> cacheMap;
    std::list<CacheEntry> cacheList;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILESYSTEM_CACHE_H
