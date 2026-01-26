// SPDX-License-Identifier: GPL-3.0-only
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFile* VirtualFilesystem::findFile(const FileID file)
{
    iterationCache.clear(); // For non-recursive iteration
    for(auto& dir : root.getDirs())
    {
        iterationCache.push_back(&dir);
    }

    while(!iterationCache.empty())
    {
        auto& first = iterationCache.back();
        auto* result = first->findFile(file);
        if(result != nullptr)
        {
            return result;
        }
        for(auto& dir : first->getDirs())
        {
            iterationCache.push_back(&dir);
        }
        iterationCache.pop_back();
    }
    return nullptr;
}

VirtualDirectory* VirtualFilesystem::getDir(const FileID dir)
{
    if(dir == root.getID())
    {
        return &root;
    }

    iterationCache.clear();
    iterationCache.push_back(&root);

    while(!iterationCache.empty())
    {
        auto& first = iterationCache.back();
        for(auto& itDir : first->getDirs())
        {
            if(itDir.getID() == dir)
            {
                return &itDir;
            }
            iterationCache.push_back(&itDir);
        }
        iterationCache.pop_back();
    }
    return nullptr;
}


bool VirtualFilesystem::dirCanHoldSize(FileID dir, uint64_t additional)
{
    auto* target = getDir(dir);
    if(target == nullptr)
    {
        return false;
    }

    iterationCache.clear();
    //VirtualDirectory* parent = target->getInfo().parent;

}

VirtualDirectory& VirtualFilesystem::getRoot()
{
    return root;
}

VirtualFilesystem::VirtualFilesystem(const DirectoryCreationInfo& info) : root(info)
{
}

VirtualFilesystem::~VirtualFilesystem() = default;

} // namespace tpunkt
