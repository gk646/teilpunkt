#ifndef TPUNKT_PERMISSION_H
#define TPUNKT_PERMISSION_H

#include <cstdint>

namespace tpunkt
{
    enum class FileOperationType : uint8_t
    {
        READ,
        WRITE,
        CREATE,
    };

    // Store permissions in a hashed way in memory with a random salt
    struct UserPermissions final
    {

    };

} // namespace tpunkt

#endif //TPUNKT_PERMISSION_H