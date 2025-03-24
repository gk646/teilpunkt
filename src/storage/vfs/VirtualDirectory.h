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
    uint32_t idx = UINT32_MAX;
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

    uint64_t sizeCurrent = 0;     // Size of all contained files in bytes

    uint32_t accessCount = 0;     // How often directory was accessed
    uint32_t changeCount = 0;     // How often directory was accessed

    uint32_t fileCount = 0;
    uint32_t dirCount = 0;

    uint32_t subdirFileCount = 0; // Files in all subdirectories only
    uint32_t subdirDirCount = 0;  // Dirs in all subdirectories only
};

struct VirtualDirectory final
{
    explicit VirtualDirectory(const DirectoryCreationInfo& info);
    TPUNKT_MACROS_MOVE_ONLY(VirtualDirectory);

    //===== Get =====//

    // Only local search - non-recursive
    VirtualFile* searchFile(FileID file);
    VirtualDirectory* searchDir(FileID dir);

    //===== Contents =====//

    bool fileAdd(const FileCreationInfo& info);
    bool fileChange(FileID file, uint64_t newSize);
    bool fileRemove(FileID fileid);
    bool fileRemoveAll();
    [[nodiscard]] bool fileExists(const FileName& name) const;

    bool dirAdd(const DirectoryCreationInfo& info);
    bool dirRemove(FileID dir); // Only works if dir is empty
    bool dirRemoveAll();
    [[nodiscard]] bool dirExists(const FileName& name) const;

    std::forward_list<VirtualDirectory, SharedBlockAllocator<VirtualDirectory>>& getDirs();

    // Returns true if a file with the given size fits into this directory - needs to check all parents
    [[nodiscard]] bool canFit(uint64_t fileSize) const;

    //===== Self =====//

    void rename(const FileName& name);

    const DirectoryStats& getStats() const;

    const DirectoryLimits& getLimits() const;

    //===== DTO =====//


    mutable CooperativeSpinlock lock;

  private:
    void onAccess() const;
    void onChange() const;

    // Returns true on change
    using DirFunc = bool (*)(VirtualDirectory& file);
    // Called for each parent up to the root
    void propagateChange(DirFunc func) const;

    DirectoryInfo info;
    mutable DirectoryStats stats;
    DirectoryPerms perms;
    DirectoryLimits limits;

    std::forward_list<VirtualFile, SharedBlockAllocator<VirtualFile>> files;
    std::forward_list<VirtualDirectory, SharedBlockAllocator<VirtualDirectory>> dirs;
};


} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H