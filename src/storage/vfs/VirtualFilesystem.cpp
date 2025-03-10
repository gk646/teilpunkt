// SPDX-License-Identifier: GPL-3.0-only

#include "instance/InstanceConfig.h"
#include "storage/Storage.h"
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFilesystem::VirtualFilesystem(const DirectoryCreationInfo& info)
{
}

VirtualFilesystem::~VirtualFilesystem()
{
}

VirtualFile* VirtualFilesystem::getFile(const FileID file)
{
    SpinlockGuard guard{systemLock};

    dirCache.clear();
    dirCache.push_back(&root);

    VirtualFile* ptr = nullptr;
    while(!dirCache.empty()) [[likely]]
    {
        auto* curr = dirCache.front();
        dirCache.pop_front();

        ptr = curr->searchFile(file);
        if(ptr != nullptr) [[unlikely]]
        {
            return ptr;
        }

        for(auto& dir : curr->getFiles())
        {
            dirCache.push_back(&dir);
        }
    }

    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getDir(const FileID file)
{
    SpinlockGuard guard{systemLock};

    dirCache.clear();
    dirCache.push_back(root);
    auto& store = GetStorage().getDirStore();

    VirtualDirectory* ptr = nullptr;
    while(!dirCache.empty()) [[likely]]
    {
        const auto idx = dirCache.front();
        dirCache.pop_front();

        auto& dir = store[ idx ]->get();
        ptr = dir.searchDir(file);
        if(ptr != nullptr) [[unlikely]]
        {
            return ptr;
        }

        for(const auto& block : dir.getDirs())
        {
            dirCache.push_back(block.getIdx());
        }
    }

    return nullptr;
}


} // namespace tpunkt