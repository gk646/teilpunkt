#ifndef TPUNKT_VIRTUAL_FILE_H
#define TPUNKT_VIRTUAL_FILE_H

#include <vector>
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


        VirtualFile(const VirtualFile&) = delete;
        VirtualFile& operator=(const VirtualFile&) = delete;

      private:
        FileInfo info;
        FileStats stats;
        FileHistory history;
    };

    struct VirtualDirectory final
    {
        FileName name;
        std::vector<VirtualFile> files;
        std::vector<VirtualDirectory> subdirectories;
    };


} // namespace tpunkt


#endif // TPUNKT_VIRTUAL_FILE_H