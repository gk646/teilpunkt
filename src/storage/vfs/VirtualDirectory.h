// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_DIRECTORY_H
#define TPUNKT_VIRTUAL_DIRECTORY_H

#include <vector>
#include "common/FileID.h"
#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{

struct DirCreationInfo final
{
    FileName name;
    UserID creator = UserID::INVALID;
    FileID parent;
    uint64_t maxSize = 0;
};

struct DirectoryInfo final
{
    FileInfo base;
    FileID parent;
};

struct DirectoryLimits final
{
    uint64_t sizeLimit = 0;
    bool isReadOnly = false;
};

struct DirectoryStats final
{
    FileStats base;
    uint64_t subDirFileSize = 0; // Size of all files in subdirs

    [[nodiscard]] uint64_t getTotalSize() const
    {
        return base.size + subDirFileSize;
    }
};

struct VirtualDirectory final
{
    VirtualDirectory(const DirCreationInfo& info);

    //===== Get =====//

    // Returns nullptr of the element identified by the given id - Only local non-recursive search
    VirtualFile* findFile(FileID file);
    VirtualDirectory* findDir(FileID dir);

    //===== Contents =====//

    // Adds a new file to this directory
    bool fileAdd(const FileCreationInfo& info, FileID& file);

    // Returns true if the size of the given file has changed
    bool fileChangeSize(FileID file, uint64_t newFileSize);

    // Returns true if the given file is deleted - deletes all its subdirs and subfiles
    bool fileDelete(FileID fileid);
    bool fileDeleteAll();

    // Returns true and assigns dir if a new directory was added
    bool dirAdd(const DirCreationInfo& info, FileID& dir);

    // Returns true if a directory with the given name exists
    [[nodiscard]] bool dirNameExists(const FileName& name) const;
    // Returns true if a file with the given name exists
    [[nodiscard]] bool fileNameExists(const FileName& name) const;

    // Returns true if a directory was removed
    bool dirDelete(FileID dir);

    //===== Self =====//

    void rename(const FileName& name);

    //===== Info =====//

    [[nodiscard]] const DirectoryStats& getStats() const;
    [[nodiscard]] const DirectoryLimits& getLimits() const;
    [[nodiscard]] FileID getID() const;
    std::vector<VirtualFile>& getFiles();
    std::vector<VirtualDirectory>& getDirs();
    void collectEntries(std::vector<DTO::DirectoryEntry>& entries) const;

    //===== DTO =====//

  private:
    [[nodiscard]] bool canHoldSizeChange(uint64_t currSize, uint64_t newSize) const;

    void onAccess();
    void onModification();
    bool fileDeleteImpl(FileID file);

    FileID fid;
    DirectoryInfo info;
    DirectoryStats stats;
    DirectoryLimits limits;
    std::vector<VirtualFile> files;
    std::vector<VirtualDirectory> dirs;
    friend DTO::DirectoryEntry;
    friend DTO::DirectoryInfo;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H
