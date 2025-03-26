// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_DIRECTORY_H
#define TPUNKT_VIRTUAL_DIRECTORY_H

#include <forward_list>
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
    FileID id;
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
    Timestamp lastEdit;
    Timestamp lastAccess;
    Timestamp creation;

    uint64_t fileSize = 0;        // Size of all files in bytes
    uint64_t subDirFileSize = 0;  // Size of all files in subdirs

    uint32_t accessCount = 0; // How often directory was accessed
    uint32_t changeCount = 0; // How often directory was accessed

    uint32_t fileCount = 0;
    uint32_t dirCount = 0;
};

struct VirtualDirectory final
{
    explicit VirtualDirectory(const DirectoryCreationInfo& info, EndpointID endpoint);
    TPUNKT_MACROS_MOVE_ONLY(VirtualDirectory);

    //===== Get =====//

    // Only local search - non-recursive
    VirtualFile* searchFile(FileID file);
    VirtualDirectory* searchDir(FileID dir);

    //===== Contents =====//

    // Returns true and assigns the id if a new file with the given info was added
    bool fileAdd(const FileCreationInfo& info, FileID& file);

    // Returns true if a size change has been made
    bool fileChange(FileID file, uint64_t newFileSize);
    bool fileExists(const FileName& name) const;
    bool fileRemove(FileID fileid);
    bool fileRemoveAll();

    bool dirAdd(const DirectoryCreationInfo& info);
    bool dirRemove(FileID dir); // Only works if dir is empty
    bool dirRemoveAll();
    bool dirExists(const FileName& name) const;

    std::forward_list<VirtualDirectory, SharedBlockAllocator<VirtualDirectory>>& getDirs();
    std::forward_list<VirtualFile, SharedBlockAllocator<VirtualFile>>& getFiles();

    //===== Self =====//

    void rename(const FileName& name);

    const DirectoryStats& getStats() const;

    const DirectoryLimits& getLimits() const;

    //===== DTO =====//


  private:
    VirtualFile* getFile(FileID file);
    bool canHoldSizeChange(uint64_t currSize, uint64_t newSize) const;

    void onAccess() const;
    void onChange() const;

    // Called for each parent up to the root
    template <typename Func>
    void iterateParents(Func func) const;

    DirectoryInfo info;
    mutable DirectoryStats stats;
    mutable Spinlock lock;
    DirectoryPerms perms;
    DirectoryLimits limits;

    std::forward_list<VirtualFile, SharedBlockAllocator<VirtualFile>> files;
    std::forward_list<VirtualDirectory, SharedBlockAllocator<VirtualDirectory>> dirs;
};


} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H