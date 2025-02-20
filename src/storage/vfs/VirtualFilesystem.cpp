// SPDX-License-Identifier: Apache License 2.0

#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

VirtualFilesystem::VirtualFilesystem(const FilesystemCreateInfo& info)
    : root(DirectoryCreationInfo{
          .name = info.name, .maxSize = info.maxSize, .parent = nullptr, .id = info.rootID, .creator = info.creator}),
      id(info.endpointID)
{
}

bool VirtualFilesystem::fileCreate(User user, FileID dir, const FileCreationInfo& info, FileID file)
{

}

VirtualFile* VirtualFilesystem::fileGet(FileID file)
{
}

VirtualFile* VirtualFilesystem::fileGetInDir(FileID file, FileID dir)
{
}

bool VirtualFilesystem::fileDelete()
{
}

VirtualDirectory* VirtualFilesystem::dirGet()
{
}

bool VirtualFilesystem::dirDelete()
{
}

bool VirtualFilesystem::dirCreate()
{
}

} // namespace tpunkt