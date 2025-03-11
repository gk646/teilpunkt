// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_SECURE_ERASER_H
#define TPUNKT_SECURE_ERASER_H

#include <sodium/utils.h>

namespace tpunkt
{
// Zeroes the given memory in its destructor - makes cleaning up objects clean and non-fallible
template <typename T>
struct SecureEraser final
{
    explicit SecureEraser(T& obj) : object(&obj)
    {
    }

    ~SecureEraser()
    {
        sodium_memzero(object, sizeof(T));
    }

  private:
    T* object;
};

template <typename Func>
struct AbortTransaction final
{

    explicit AbortTransaction(Func func) : func(func)
    {
    }

    ~AbortTransaction()
    {
        if(aborted)
        {
            func();
        }
    }

    void commit()
    {
        aborted = true;
    }

  private:
    Func func;
    bool aborted = false;
};

} // namespace tpunkt

#endif // TPUNKT_SECURE_ERASER_H