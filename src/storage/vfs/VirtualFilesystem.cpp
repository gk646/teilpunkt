// SPDX-License-Identifier: GPL-3.0-only
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFilesystem::VirtualFilesystem(const DirectoryCreationInfo& info) : root(info)
{
}

VirtualFilesystem::~VirtualFilesystem()
{
}

VirtualFile* VirtualFilesystem::getFile(const FileID file)
{
    iterationCache.clear(); // For non recursive iteration
    for(auto& dir : root.getDirs())
    {
        iterationCache.push_back(dir);
    }

    while(!iterationCache.empty())
    {
        auto& first = iterationCache.front();
        for(auto& itFile : first.getFiles())
        {
            if(itFile.getID() == file)
            {
                return &itFile;
            }
        }
        for(auto& dir : first.getDirs())
        {
            iterationCache.push_back(dir);
        }
        iterationCache.pop_front();
    }
    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getDir(const FileID dir)
{
    iterationCache.clear();
    for(auto& itDir : root.getDirs())
    {
        iterationCache.push_back(itDir);
    }

    while(!iterationCache.empty())
    {
        auto& first = iterationCache.front();
        for(auto& itDir : first.getDirs())
        {
            if(itDir.getID() == dir)
            {
                return &itDir;
            }
            iterationCache.push_back(itDir);

        }
        iterationCache.pop_front();
    }
    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getFileDir(FileID file)
{
}

} // namespace tpunkt
