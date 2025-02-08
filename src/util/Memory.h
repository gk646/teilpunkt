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


#endif // TPUNKT_MEMORY_H