// SPDX-License-Identifier: GPL-3.0-only

#include "instance/InstanceConfig.h"
#include "storage/Storage.h"
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFilesystem::VirtualFilesystem(const DirectoryCreationInfo& info, const EndpointID endpoint)
{
    root = SharedBlockAllocator<VirtualDirectory>{}.allocate(1);
    new(root) VirtualDirectory(info, endpoint);
}

VirtualFilesystem::~VirtualFilesystem()
{
}

VirtualFile* VirtualFilesystem::getFile(const FileID file)
{
    SpinlockGuard guard{systemLock};

    dirCache.clear();
    dirCache.push_back(root);

    VirtualFile* ptr = nullptr;
    while(!dirCache.empty()) [[likely]]
    {
        VirtualDirectory* curr = dirCache.front();
        dirCache.pop_front();

        ptr = curr->searchFile(file);
        if(ptr != nullptr) [[unlikely]]
        {
            return ptr;
        }

        for(auto& dir : curr->getDirs())
        {
            dirCache.push_back(&dir);
        }
    }

    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getDir(const FileID dir)
{
    SpinlockGuard guard{systemLock};
    dirCache.clear();
    dirCache.push_back(root);

    VirtualDirectory* ptr = nullptr;
    while(!dirCache.empty()) [[likely]]
    {
        auto* curr = dirCache.front();
        dirCache.pop_front();

        ptr = curr->searchDir(dir);
        if(ptr != nullptr) [[unlikely]]
        {
            return ptr;
        }

        for(auto& savedDir : curr->getDirs())
        {
            dirCache.push_back(&savedDir);
        }
    }

    return nullptr;
}


} // namespace tpunkt
