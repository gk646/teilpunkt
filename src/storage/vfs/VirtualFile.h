// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_VIRTUAL_FILE_H
#define TPUNKT_VIRTUAL_FILE_H

#include "common/FileID.h"
#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"
#include "fwd.h"

namespace tpunkt
{

// Provided at creation
struct FileCreationInfo final
{
    FileName name;
    UserID creator = UserID::INVALID;
    EndpointID endpoint;
};

struct FileInfo final
{
    FileName name;
    UserID creator = UserID::INVALID;
    UserID owner = UserID::INVALID;
};

struct FileStats final
{
    Timestamp created;
    Timestamp modified; // Last time the physical file OR metadata changed
    Timestamp accessed; // Last time the physical file was sent (or attempted)

    uint32_t modificationCount = 0;
    uint32_t accessCount = 0;

    uint64_t size = 0;  // Size in bytes
};

struct FileHistory final
{
    bool enabled = false;
};

struct VirtualFile final
{
    explicit VirtualFile(const FileCreationInfo& info);

    //===== Change =====//

    // Changes the name to the given name
    // Note: Does NOT rename the physical file
    void rename(const FileName& newName);

    //===== Info =====//

    [[nodiscard]] const FileInfo& getInfo() const;
    [[nodiscard]] const FileStats& getStats() const;
    [[nodiscard]] FileID getID() const;

  private:
    void onAccess();
    void onModification();

    FileID fid;
    FileInfo info;
    FileStats stats{};
    FileHistory history{};
    friend VirtualDirectory;
    friend DTO::ResponseDirectoryEntry;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILE_H
