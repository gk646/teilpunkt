// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_VIRTUAL_FILE_H
#define TPUNKT_VIRTUAL_FILE_H

#include "datastructures/FixedString.h"
#include "datastructures/Spinlock.h"
#include "datastructures/Timestamp.h"
#include "fwd.h"

namespace tpunkt
{

// Provided at creation
struct FileCreationInfo final
{
    FileName name;
    UserID creator;
};

struct FileInfo final
{
    FileName name;
    UserID creator = UserID::INVALID;
    FileID id;
};

struct FileStats final
{
    Timestamp lastEdit;
    Timestamp creation;
    uint32_t changeCount = 0;
    uint32_t accessCount = 0;

    uint64_t size = 0; // Size in bytes
};

struct FileHistory final
{
    bool enabled = false;
};

struct VirtualFile final
{
    explicit VirtualFile(const FileCreationInfo& info);
    TPUNKT_MACROS_MOVE_ONLY(VirtualFile);

  private:
    FileInfo info;
    FileStats stats{};
    FileHistory history{};
    CooperativeSpinlock lock;
    friend Storage;
    friend VirtualDirectory;
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILE_H