// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_DIRECTORY_H
#define TPUNKT_VIRTUAL_DIRECTORY_H

#include <vector>
#include "common/FileID.h"
#include "datastructures/BlockAllocator.h"
#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{

struct DirectoryCreationInfo final
{
    FileName name;
    uint64_t maxSize = 0;
    VirtualDirectory* parent = nullptr;
    UserID creator = UserID::INVALID;
};

struct DirectoryInfo final
{
    FileName name;
    VirtualDirectory* parent = nullptr; // Only null if it's the root of an endpoint
    bool isHidden = false;
};

struct DirectoryLimits final
{
    uint64_t sizeLimit = 0;
    bool isReadOnly = false;
};

struct DirectoryPerms final
{
    UserID creator = UserID::INVALID;
    UserID owner = UserID::INVALID;
};

struct DirectoryStats final
{
    Timestamp created;
    Timestamp modified;          // Last time any physical file OR metadata changes
    Timestamp accessed;          // Last time any physical file was sent

    uint32_t modificationCount = 0;
    uint32_t accessCount = 0;

    uint64_t fileSize = 0;       // Size of all files in bytes
    uint64_t subDirFileSize = 0; // Size of all files in subdirs

    uint64_t getTotalSize() const
    {
        return fileSize + subDirFileSize;
    }
};

struct VirtualDirectory final
{
    VirtualDirectory(const DirectoryCreationInfo& info);

    //===== Get =====//

    // Returns nullptr of the element identified by the given id - Only local non-recursive search
    VirtualFile* findFile(FileID file);
    VirtualDirectory* findDir(FileID dir);

    //===== Contents =====//

    // Adds a new file to this directory
    bool fileAdd(const FileCreationInfo& info, FileID& file);

    // Returns true if the size of the given file has changed
    bool fileChange(FileID file, uint64_t newFileSize);

    // Returns true if the given file is deleted - deletes all its subdirs and subfiles
    bool fileDelete(FileID fileid);
    bool fileDeleteAll();

    // Returns true and assigns dir if a new directory was added
    bool dirAdd(const DirectoryCreationInfo& info, FileID& dir);

    // Returns true if a directory with the given name exists
    [[nodiscard]] bool dirNameExists(const FileName& name) const;
    // Returns true if a file with the given name exists
    [[nodiscard]] bool fileNameExists(const FileName& name) const;

    // Returns true if a directory was removed
    bool dirDelete(FileID dir);
    bool dirDeleteAll();

    //===== Self =====//

    void rename(const FileName& name);

    //===== Info =====//

    [[nodiscard]] const DirectoryStats& getStats() const;
    [[nodiscard]] const DirectoryLimits& getLimits() const;
    [[nodiscard]] FileID getID() const;
    std::vector<VirtualFile>& getFiles();
    std::vector<VirtualDirectory>& getDirs();

    //===== DTO =====//

  private:
    [[nodiscard]] bool canHoldSizeChange(uint64_t currSize, uint64_t newSize) const;

    void onAccess();
    void onModification();

    // Called for each parent up to the root
    template <typename Func>
    void iterateParents(Func func) const;

    bool fileDeleteImpl(FileID file);

    FileID id;
    DirectoryInfo info;
    DirectoryStats stats;
    DirectoryPerms perms;
    DirectoryLimits limits;
    std::vector<VirtualFile> files;
    std::vector<VirtualDirectory> dirs;
};

template <typename Func>
void VirtualDirectory::iterateParents(Func func) const
{
    VirtualDirectory* current = info.parent;

    // Iterate up with locking
    while(current != nullptr)
    {
        const auto changed = func(*current);
        if(changed)
        {
            current->onModification();
        }
        current = current->info.parent;
    }

    // Iterate up to unlock
    current = info.parent;
    while(current != nullptr)
    {
        current = current->info.parent;
    }
}


} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H
