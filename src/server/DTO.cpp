// SPDX-License-Identifier: GPL-3.0-only
#include "auth/Authenticator.h"
#include "server/DTO.h"
#include "storage/vfs/VirtualDirectory.h"
#include "storage/vfs/VirtualFile.h"

namespace tpunkt::DTO
{

ResponseDirectoryInfo ResponseDirectoryInfo::FromDir(const VirtualDirectory& dir)
{
    ResponseDirectoryInfo info{};
    info.name = dir.info.base.name;
    info.fid = dir.fid;
    return info;
}

ResponseDirectoryEntry ResponseDirectoryEntry::FromFile(const VirtualFile& file)
{
    ResponseDirectoryEntry entry{};
    entry.name = file.info.name;
    entry.fid = file.fid;
    entry.isFile = true;

    const FileInfo& info = file.info;
    const FileStats& stats = file.stats;

    auto& auth = Authenticator::GetInstance();
    auth.getUserName(info.creator, entry.creator);
    auth.getUserName(info.owner, entry.owner);

    entry.unixLastEdit = stats.modified.getSeconds();
    entry.unixLastAccess = stats.accessed.getSeconds();
    entry.unixCreation = stats.created.getSeconds();

    entry.sizeBytes = stats.size;
    return entry;
}

ResponseDirectoryEntry ResponseDirectoryEntry::FromDir(const VirtualDirectory& dir)
{
    ResponseDirectoryEntry entry{};
    entry.name = dir.info.base.name;
    entry.fid = dir.fid;
    entry.isFile = false;

    const FileInfo& info = dir.info.base;
    const FileStats& stats = dir.stats.base;

    auto& auth = Authenticator::GetInstance();
    auth.getUserName(info.creator, entry.creator);
    auth.getUserName(info.owner, entry.owner);

    entry.unixLastEdit = stats.modified.getSeconds();
    entry.unixLastAccess = stats.accessed.getSeconds();
    entry.unixCreation = stats.created.getSeconds();

    entry.sizeBytes = dir.stats.getTotalSize();
    return entry;
}

} // namespace tpunkt::DTO
