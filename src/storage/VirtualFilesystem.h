#ifndef TPUNKT_VIRTUAL_FILESYSTEM_H
#define TPUNKT_VIRTUAL_FILESYSTEM_H

#include <vector>
#include <cstdint>
#include "datastructures/FixedString.h"
#include "config.h"

namespace tpunkt
{
    struct VirtualFile final
    {
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        uint64_t encryptedSize{};
    };

    struct VirtualDirectory final
    {
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        std::vector<VirtualFile> files;
        std::vector<VirtualDirectory> subdirectories;
    };

    struct VirtualFilesystem
    {
        VirtualDirectory root;

        VirtualFilesystem();
    };

} // namespace tpunkt


#endif //TPUNKT_VIRTUAL_FILESYSTEM_H