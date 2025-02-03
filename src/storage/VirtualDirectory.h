#ifndef TPUNKT_VIRTUAL_DIRECTORY_H
#define TPUNKT_VIRTUAL_DIRECTORY_H

#include <vector>
#include "storage/VirtualFile.h"

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
    };

    // Currently built using vectors - not cache and fragmentation friendly - done for simplicity - can be changed
    struct VirtualDirectory final
    {
        explicit VirtualDirectory(const DirectoryCreationInfo& info);
        VirtualDirectory& operator=(VirtualDirectory&&) = default;
        VirtualDirectory (VirtualDirectory&&) = default;

        VirtualDirectory(const VirtualDirectory&) = delete;
        VirtualDirectory& operator=(const VirtualDirectory&) = delete;

        //===== Manipulation =====//

        // Does not check if the file fits - passed for explicitness that its checked first
        bool fileAdd(const FileCreationInfo& info, bool fits, bool unique);

        bool fileRemove(FileID fileID);

        bool directoryAdd(const DirectoryCreationInfo& info);

        bool directoryRemove(FileID fileID);

        //===== Information =====//

        // Returns true if a file with the given size fits into this directory - needs to check all parents
        [[nodiscard]] bool canFit(uint64_t fileSize) const;

        // Returns true if the given name is an existing file or directory name - case-sensitive
        [[nodiscard]] bool nameExists(const FileName& name) const;

        [[nodiscard]] uint32_t getFileCount() const;
        [[nodiscard]] uint32_t getDirCount() const;

      private:
        void propagateAdd(uint64_t size);
        void propagateRemove(uint64_t size);

        DirectoryInfo info;
        DirectoryStats stats;
        std::vector<VirtualFile> files;
        std::vector<VirtualDirectory> subdirectories;
    };


} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H