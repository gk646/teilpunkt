#ifndef TPUNKT_VIRTUAL_FILE_H
#define TPUNKT_VIRTUAL_FILE_H

#include <vector>
#include "fwd.h"
#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"

namespace tpunkt
{

    //===== Creation Info =====//

    // Provided at creation
    struct FileCreationInfo final
    {
        FileName name;
        UserID creator = UserID::INVALID;
        uint64_t size{};
    };


    struct DirectoryCreationInfo final
    {
    };


    //===== Virtual File =====//

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
        const Timestamp creation;
        uint32_t version{};     // How often file was changed
        uint32_t accessCount{}; // How often file was used
    };

    struct FileHistory final
    {
        std::vector<FileName> pastNames;
        bool enabled = false;
    };

    struct VirtualFile final
    {
        explicit VirtualFile(const FileCreationInfo& info, FileID file);

        VirtualFile(const VirtualFile&) = delete;
        VirtualFile& operator=(const VirtualFile&) = delete;

      private:
        FileInfo info;
        FileStats stats{};
        FileHistory history{};
        friend Storage;
        friend VirtualDirectory;
    };

    // Currently built using vectors - not cache and fragmentation friendly - done for simplicity - not fixed
    struct VirtualDirectory final
    {
        explicit VirtualDirectory(const FileName& name, uint64_t maxSize, VirtualDirectory* parent);

        bool fileAdd(const FileCreationInfo& info, FileID file);

        bool fileRemove(FileID fileID);

        // Returns true if file fits into this directory - checking this dir is enough
        // Will always have at least equal or less sizeLimit than parent
        [[nodiscard]] bool fileFits(const FileCreationInfo& info) const;

        bool directoryAdd();

        VirtualDirectory(const VirtualDirectory&) = delete;
        VirtualDirectory& operator=(const VirtualDirectory&) = delete;

      private:
        void propagateSizeChange(uint64_t change, bool subtract);

        FileName name;
        std::vector<VirtualFile> files;
        std::vector<VirtualDirectory> subdirectories;
        VirtualDirectory* parent = nullptr; // Only null if it's the root of a endpoint
        uint64_t sizeLimit{};
        uint64_t sizeCurrent{};
    };


} // namespace tpunkt


#endif // TPUNKT_VIRTUAL_FILE_H