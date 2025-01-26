#ifndef TPUNKT_WRAPPED_KEY_H
#define TPUNKT_WRAPPED_KEY_H

#include "fwd.h"

namespace tpunkt
{
    struct WrappedKey final
    {
        int key;
        FileID fileHandle;
    };

} // namespace tpunkt


#endif // TPUNKT_WRAPPED_KEY_H