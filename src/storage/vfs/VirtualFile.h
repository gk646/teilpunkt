// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_VIRTUAL_FILE_H
#define TPUNKT_VIRTUAL_FILE_H

#include "fwd.h"
#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"

namespace tpunkt
{
    // Provided at creation
    struct FileCreationInfo final
    {
        FileName name;
        UserID creator = UserID::INVALID;
        uint64_t size{};
        FileID id;
    };

    struct FileInfo final
    {
        FileName name;
        UserID creator = UserID::INVALID;
        uint64_t size{};        // Size in bytes
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

        VirtualFile(VirtualFile&&) = default;
        VirtualFile& operator=(VirtualFile&&) = default;

        VirtualFile(const VirtualFile&) = delete;
        VirtualFile& operator=(const VirtualFile&) = delete;

      private:
        FileInfo info;
        FileStats stats{};
        FileHistory history{};
        friend Storage;
        friend VirtualDirectory;
    };


} // namespace tpunkt


#endif // TPUNKT_VIRTUAL_FILE_H