// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_DIRECTORY_H
#define TPUNKT_VIRTUAL_DIRECTORY_H

#include "datastructures/BlockStorage.h"
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

    uint32_t subdirFileCount = 0; // Files in all subdirectories only
    uint32_t subdirDirCount = 0;  // Dirs in all subdirectories only
};

struct VirtualDirectory final
{
    explicit VirtualDirectory(const DirectoryCreationInfo& info);
    ~VirtualDirectory();
    TPUNKT_MACROS_MOVE_ONLY(VirtualDirectory);

    //===== Get =====//

    // Only local search - non-recursive
    [[nodiscard]] VirtualFile* searchFile(FileID file);
    [[nodiscard]] VirtualDirectory* searchDir(FileID dir);

    const BlockList<VirtualDirectory>& getDirs() const;

    //===== Contents =====//

    bool fileAdd(const FileCreationInfo& info, uint32_t& idx);
    bool fileRemove(FileID fileid);
    bool fileRemoveAll();
    [[nodiscard]] bool fileExists(const FileName& name) const;

    bool dirAdd(const DirectoryCreationInfo& info, uint32_t& idx);
    bool dirRemove(FileID dir); // Only works if dir is empty
    bool dirRemoveAll();
    [[nodiscard]] bool dirExists(const FileName& name) const;

    // Returns true if a file with the given size fits into this directory - needs to check all parents
    [[nodiscard]] bool canFit(uint64_t fileSize) const;

    //===== Self =====//

    bool rename(const FileName& name);

    //===== DTO =====//

    void dtoInfo(DTOFileInfo& info);
    void dtoStats(DTOFileStats& stats);

    CooperativeSpinlock lock;

  private:
    using DirFunc = void (*)(VirtualDirectory& file);
    // Called for each parent up to the root
    void propagateChange(DirFunc func) const;

    DirectoryInfo info;
    DirectoryStats stats;
    DirectoryPerms perms;
    DirectoryLimits limits;

    BlockList<VirtualFile> files;
    BlockList<VirtualDirectory> dirs;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H