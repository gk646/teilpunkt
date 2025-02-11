// SPDX-License-Identifier: Apache License 2.0

#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{
    VirtualFile::VirtualFile(const FileCreationInfo& info) : info(info.name, info.creator, info.size, info.id)
    {
    }

} // namespace tpunkt