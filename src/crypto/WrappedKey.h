#ifndef TPUNKT_WRAPPED_KEY_H
#define TPUNKT_WRAPPED_KEY_H

#include "common/FileHandle.h"

namespace tpunkt
{
    struct WrappedKey final
    {
        int key;
        FileHandle fileHandle;
    };

} // namespace tpunkt


#endif // TPUNKT_WRAPPED_KEY_H