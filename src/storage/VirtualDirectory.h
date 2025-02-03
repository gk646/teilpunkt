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
    };

    // Currently built using vectors - not cache and fragmentation friendly - done for simplicity - can be changed
    struct VirtualDirectory final
    {
        explicit VirtualDirectory(const DirectoryCreationInfo& info);
        VirtualDirectory(const VirtualDirectory&) = delete;
        VirtualDirectory& operator=(const VirtualDirectory&) = delete;

        //===== Manipulation =====//

        // Does not check if the file fits - passed for explicitness that its checked first
        bool fileAdd(const FileCreationInfo& info, FileID file, bool fits, bool unique);

        bool directoryAdd(const DirectoryCreationInfo& info);

        bool fileRemove(FileID fileID);

        //===== Information =====//

        // Returns true if a file with the given size fits into this directory - needs to check all parents
        [[nodiscard]] bool canFit(uint64_t fileSize) const;

        // Returns true if the given name is an existing file or directory name - case-sensitive
        [[nodiscard]] bool nameExists(const FileName& name) const;

      private:
        void propagateSizeChange(uint64_t change, bool subtract);

        FileName name;
        std::vector<VirtualFile> files;
        std::vector<VirtualDirectory> subdirectories;
        VirtualDirectory* parent = nullptr; // Only null if it's the root of an endpoint
        uint64_t sizeLimit = 0;
        uint64_t sizeCurrent = 0;
        FileID id;
        uint32_t containedFiles = 0;
    };


} // namespace tpunkt

#endif // TPUNKT_VIRTUAL_DIRECTORY_H