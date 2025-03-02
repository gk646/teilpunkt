// SPDX-License-Identifier: Apache License 2.0

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
    FileID id{};
    UserID creator = UserID::INVALID;
};

struct DirectoryInfo final
{
    FileName name;
    uint64_t sizeLimit = 0;
    uint64_t sizeCurrent = 0;
    FileID id;
    VirtualDirectory* parent = nullptr; // Only null if it's the root of an endpoint
    UserID creator = UserID::INVALID;
};

struct DirectoryStats final
{
    Timestamp lastEdit;
    Timestamp creation;
    uint32_t accessCount{};             // How often directory was accessed
    uint32_t changeCount{};             // How often directory was accessed
    uint32_t subdirFileCount;           // Files contained in all subdirs (only those)
    uint32_t subdirDirCount;            // Dirs contained in all subdirs (only those)
};

// Currently built using vectors - not cache and fragmentation friendly - done for simplicity - can be changed
struct VirtualDirectory final
{
    explicit VirtualDirectory(const DirectoryCreationInfo& info);
    ~VirtualDirectory();
    TPUNKT_MACROS_STRUCT(VirtualDirectory);

    //===== Get =====//

    // Only local search - non-recursive
    [[nodiscard]] VirtualFile* searchFile(FileID file) const;
    [[nodiscard]] VirtualDirectory* searchDir(FileID dir) const;

    //=====  Manipulation =====//

    bool addFile(const FileCreationInfo& info);

    bool removeFile(FileID fileid);

    bool addDirectory(const DirectoryCreationInfo& info);

    // Only works if dir is empty
    bool removeDirectory(FileID dir);

    //===== Content Manipulation Misc =====//

    // Only for this dir - removes files individually
    bool removeAllFiles();

    //===== Self Manipulation =====//

    bool rename(const FileName& name);

    // Does not check authentication
    bool changeOwner(UserID owner, bool authenticated);

    //===== Information =====//

    // Returns true if a file with the given size fits into this directory - needs to check all parents
    [[nodiscard]] bool canFit(uint64_t fileSize) const;

    // Returns true if the given name is an existing file or directory name - case-sensitive
    [[nodiscard]] bool nameExists(const FileName& name) const;

    // Only in this directory
    [[nodiscard]] uint32_t getFileCount() const;
    [[nodiscard]] uint32_t getDirCount() const;

    // Includes all subdirectories (recursive)
    [[nodiscard]] uint32_t getTotalFileCount() const;
    [[nodiscard]] uint32_t getTotalDirCount() const;

    CooperativeSpinlock lock;

  private:
    void propagateAddFile(uint64_t size);
    void propagateRemoveFile(uint64_t size);
    void propagateAddDir() const;
    void propagateRemoveDir() const;

    DirectoryInfo info;
    DirectoryStats stats;

    BlockNode<VirtualFile>* startFile = nullptr;
    BlockNode<VirtualDirectory>* startDir = nullptr;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H