// SPDX-License-Identifier: Apache License 2.0

#include "instance/InstanceConfig.h"
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFilesystem::VirtualFilesystem(const DirectoryCreationInfo& info)
    : cache(GetInstanceConfig().getNumber(NumberParamKey::STORAGE_MAX_TOTAL_FILES_OR_DIRS))
{
}

VirtualFilesystem::~VirtualFilesystem()
{
}

VirtualFile* VirtualFilesystem::getFile(const FileID file)
{
    VirtualFile* ptr = cache.searchFile(file);
    if(ptr != nullptr)
    {
        return ptr;
    }

    SpinlockGuard guard{systemLock};

    dirCache.clear();
    dirCache.push_back(root);

    while(!dirCache.empty()) [[likely]]
    {
        const auto* dir = dirCache.front();
        dirCache.pop_front();

        ptr = dir->get().searchFile(file);
        if(ptr != nullptr) [[unlikely]]
        {
            return ptr;
        }
        // TODO file and dir iterator in virtual dir
    }


    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getDir(const FileID dir)
{
    VirtualDirectory* ptr = cache.searchDir(dir);
    if(ptr != nullptr)
    {
        return ptr;
    }


    dirCache.clear();
    dirCache.push_back(root);

    while(!dirCache.empty()) [[likely]]
    {
        const auto* currentDir = dirCache.front();
        dirCache.pop_front();

        ptr = currentDir->get().searchDir(dir);
        if(ptr != nullptr) [[unlikely]]
        {
            return ptr;
        }
    }
    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getFileDir(FileID file)
{
}


} // namespace tpunkt