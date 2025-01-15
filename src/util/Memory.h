#ifndef TPUNKT_MEMORY_H
#define TPUNKT_MEMORY_H

#define TPUNKT_ALLOC(var, size)                                                                                        \
    static_cast<decltype(var)>(malloc(size < 0 ? 0 : size));                                                           \
    if(var == nullptr || size < 0)                                                                                     \
    LOG_FATAL("Error allocating")

#define TPUNKT_FREE(ptr) free(ptr)


#define TPUNKT_SECUREALLOC(var, size)                                                                                  \
    static_cast<decltype(var)>(sodium_malloc(size < 0 ? 0 : size));                                                    \
    if(var == nullptr || size < 0)                                                                                     \
        LOG_FATAL("Error allocating");                                                                                 \
    sodium_mlock(var, size)


#define TPUNKT_SECUREFREE(ptr) sodium_free(ptr)


#endif // TPUNKT_MEMORY_H