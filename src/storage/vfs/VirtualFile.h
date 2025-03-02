// SPDX-License-Identifier: Apache License 2.0

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
    FileID id;
    UserID creator;
};

struct FileInfo final
{
    FileName name;
    UserID creator = UserID::INVALID;
    uint64_t size = 0;      // Size in bytes
    FileID id;
};

struct FileStats final
{
    Timestamp lastEdit;
    Timestamp creation;
    uint32_t version{};     // How often file was changed
    uint32_t accessCount{}; // How often file was used
};

struct FileHistory final
{
    bool enabled = false;
};

struct VirtualFile final
{
    explicit VirtualFile(const FileCreationInfo& info);

  private:
    FileInfo info;
    FileStats stats{};
    FileHistory history{};
    CooperativeSpinlock lock;
    friend Storage;
    friend VirtualDirectory;
    TPUNKT_MACROS_STRUCT(VirtualFile);
};

} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_FILE_H