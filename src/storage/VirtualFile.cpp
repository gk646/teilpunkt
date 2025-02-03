#include "storage/VirtualFile.h"

namespace tpunkt
{
    VirtualFile::VirtualFile(const FileCreationInfo& info) : info(info.name, info.creator, info.size, info.id)
    {
    }

} // namespace tpunkt