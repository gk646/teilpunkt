#ifndef TPUNKT_SECURE_LIST_H
#define TPUNKT_SECURE_LIST_H

#include <sodium/utils.h>
#include "util/Macros.h"

namespace tpunkt
{

    template <typename T>
    struct SecureList final
    {
        TPUNKT_MACROS_STRUCT(SecureList);

        T* val = nullptr;
        int size = 0;
        int capacity = 0;


        struct ListReader final
        {
            SecureList& list;

            explicit ListReader(SecureList& box) : list(box) {}
            ~ListReader() {}

            TPUNKT_MACROS_STRUCT(ListReader);
        };

        explicit SecureList(const int initialCapacity = 10)
        {
            val = TPUNKT_SECUREALLOC(val, sizeof(T) * initialCapacity);
            sodium_mlock(val, sizeof(T) * initialCapacity);
            sodium_mprotect_noaccess(val);
        }

        ~SecureList()
        {
            sodium_mprotect_readwrite(val);
            sodium_munlock(val, sizeof(T) * capacity);
            TPUNKT_SECUREFREE(val);
        }

        ListReader get() { return ListReader{this}; }

    private:
        void grow(int newCapacity)
        {
            if (newCapacity <= capacity)
            {
                return;
            }
        }
    };


} // namespace tpunkt

#endif //TPUNKT_SECURE_LIST_H