// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_MEMORY_H
#define TPUNKT_MEMORY_H

#include <cstdlib>

#define TPUNKT_ALLOC(var, size)                                                                                        \
    static_cast<decltype(var)>(malloc(size));                                                                          \
    if(var == nullptr)                                                                                                 \
    {                                                                                                                  \
        LOG_FATAL("Error allocating");                                                                                 \
    }

#define TPUNKT_REALLOC(var, size)                                                                                      \
    static_cast<decltype(var)>(realloc(var, size));                                                                    \
    if(var == nullptr)                                                                                                 \
    {                                                                                                                  \
        LOG_FATAL("Error allocating");                                                                                 \
    }


#define TPUNKT_FREE(ptr) free(ptr);


#define TPUNKT_SECUREALLOC(var, size)                                                                                  \
    static_cast<decltype(var)>(sodium_malloc(size));                                                                   \
    if(var == nullptr)                                                                                                 \
    {                                                                                                                  \
        LOG_FATAL("Error allocating");                                                                                 \
    }                                                                                                                  \
    sodium_mlock(var, size)


#define TPUNKT_SECUREFREE(ptr) sodium_free(ptr);


#define TPUNKT_MMAP(ptr, size, prot, flags)                                                                            \
    static_cast<decltype(ptr)>(mmap(nullptr, size, prot, flags, -1, 0));                                               \
    if(ptr == MAP_FAILED)                                                                                              \
    {                                                                                                                  \
        LOG_FATAL("Error allocation with mmap");                                                                       \
    }


#define TPUNKT_MUNMAP(ptr, size)                                                                                       \
    const auto result = munmap(ptr, size);                                                                             \
    if(result == -1)                                                                                                   \
    {                                                                                                                  \
        LOG_FATAL("Error freeing with munmap");                                                                        \
    }


#endif // TPUNKT_MEMORY_H
