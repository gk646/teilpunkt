// SPDX-License-Identifier: GPL-3.0-only

#include "storage/vfs/VirtualFile.h"

namespace tpunkt
{

VirtualFile::VirtualFile(const FileCreationInfo& info) : info(info.name, info.creator)
{
}

} // namespace tpunkt