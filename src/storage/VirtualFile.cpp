#include "storage/VirtualFile.h"

#include "Storage.h"

namespace tpunkt
{
    VirtualFile::VirtualFile(const FileCreationInfo& info, const FileID file)
        : info(info.name, info.creator, info.size, file)
    {
    }

} // namespace tpunkt