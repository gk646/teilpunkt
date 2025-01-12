#ifndef TPUNKT_SECURE_BOX_H
#define TPUNKT_SECURE_BOX_H

#include <sodium/utils.h>
#include "fwd.h"
#include "util/Logger.h"
#include "util/Memory.h"
#include "util/Macros.h"

namespace tpunkt
{

    template <typename T>
    struct SecureBox final
    {
        T* val = nullptr;

        template <typename... Args>
        SecureBox()
        {
           val = TPUNKT_SECUREALLOC(val,sizeof(T*));
        }

        ~SecureBox() { sodium_free(val); }
    };


    template <typename T>
    struct BoxReader final
    {
        SecureBox<T>& box;
        explicit BoxReader(SecureBox<T>& box) : box(box) {}
        ~BoxReader() { sodium_mprotect_noaccess(box.val); }

        TPUNKT_MACROS_DEL_CTORS(BoxReader);
    };

} // namespace tpunkt
#endif //TPUNKT_SECURE_BOX_H