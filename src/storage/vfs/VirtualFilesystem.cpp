// SPDX-License-Identifier: GPL-3.0-only
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFile* VirtualFilesystem::findFile(const FileID file)
{
    iterationCache.clear();
    iterationCache.push_back(&root);

    while(!iterationCache.empty())
    {
        auto* first = iterationCache.back();
        iterationCache.pop_back();

        auto* result = first->findFile(file);
        if(result != nullptr)
        {
            return result;
        }
        for(auto& dir : first->getDirs())
        {
            iterationCache.push_back(&dir);
        }
    }
    return nullptr;
}

VirtualDirectory* VirtualFilesystem::findDir(const FileID dir)
{
    if(dir == root.getID())
    {
        return &root;
    }

    iterationCache.clear();
    iterationCache.push_back(&root);

    while(!iterationCache.empty())
    {
        auto* first = iterationCache.back();
        iterationCache.pop_back();

        for(auto& itDir : first->getDirs())
        {
            if(itDir.getID() == dir)
            {
                return &itDir;
            }
            iterationCache.push_back(&itDir);
        }
    }
    return nullptr;
}

VirtualDirectory* VirtualFilesystem::findContainingDir(FileID file)
{
    iterationCache.clear();
    iterationCache.push_back(&root);

    while(!iterationCache.empty())
    {
        auto* first = iterationCache.back();
        iterationCache.pop_back();

        auto* fileResult = first->findFile(file);
        if(fileResult != nullptr) [[unlikely]]
        {
            return first;
        }
        auto* dirResult = first->findDir(file);
        if(dirResult != nullptr) [[unlikely]]
        {
            return first;
        }
        for(auto& itDir : first->getDirs())
        {
            iterationCache.push_back(&itDir);
        }
    }
    return nullptr;
}

VirtualDirectory& VirtualFilesystem::getRoot()
{
    return root;
}

bool VirtualFilesystem::dirCanHoldSize(FileID dir, uint64_t additional)
{
    auto* target = findDir(dir);
    if(target == nullptr)
    {
        return false;
    }

    iterationCache.clear();
    // VirtualDirectory* parent = target->getInfo().parent;
}

VirtualFilesystem::VirtualFilesystem(const DirectoryCreationInfo& info) : root(info)
{
    iterationCache.reserve(64);
}

VirtualFilesystem::~VirtualFilesystem() = default;

} // namespace tpunkt
