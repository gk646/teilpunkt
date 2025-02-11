// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_PERMISSIONS_H
#define TPUNKT_PERMISSIONS_H

#include <cstdint>

namespace tpunkt
{
    enum class PermissionFlag : uint8_t
    {
        READ = 0b001,
        WRITE = 0b010,
        DELETE = 0b100,
        // Combined flags
        ALL = 0b0111,
    };


    struct UserLimits final
    {

    };

} // namespace tpunkt

#endif // TPUNKT_PERMISSIONS_H