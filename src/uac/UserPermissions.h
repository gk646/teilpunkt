// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_PERMISSIONS_H
#define TPUNKT_PERMISSIONS_H

#include <cstdint>

namespace tpunkt
{

enum class PermissionFlag : uint8_t
{
    INVALID = 0,
    READ = 1 << 1,
    DELETE = 1 << 3,
    CREATE = 1 << 4,
    WRITE = 1 << 5,
};


struct UserLimits final
{
    uint32_t ownedFiles = 0;
    uint32_t ownedDirs = 0;
    uint64_t combinedFileSize = 0;
    uint64_t maxSingleFileSize = 0;

    // Actions
    uint32_t activeActions = 0;
};

} // namespace tpunkt

#endif // TPUNKT_PERMISSIONS_H
