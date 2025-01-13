#ifndef TPUNKT_MACROS_H
#define TPUNKT_MACROS_H


#define TPUNKT_MACROS_DEL_CTORS(type)                                                                                   \
    type(type&&) = delete;                                                                                              \
    type(const type&) = delete;                                                                                         \
    type& operator=(const type&) = delete;                                                                              \
    type& operator=(const type&&) = delete;

#define TPUNKT_MACROS_STRUCT(type) TPUNKT_MACROS_DEL_CTORS(type)


#define TPUNKT_MACROS_GLOBAL_ASSIGN(var)                                                                                \
    if (var == nullptr)                                                                                                 \
        var = this;                                                                                                     \
    else                                                                                                                \
        LOG_FATAL("%s is already assigned", #var);

#define TPUNKT_MACROS_GLOBAL_RESET(var)                                                                                 \
    if (var != nullptr)                                                                                                 \
        var = nullptr;                                                                                                  \
    else                                                                                                                \
        LOG_FATAL("%s is not assigned", #var);

#define TPUNKT_MACROS_GLOBAL_GET(var)                                                                                   \
    if (var != nullptr)                                                                                                 \
        return *var;                                                                                                    \
    LOG_FATAL("%s is not assigned", #var);


#endif //TPUNKT_MACROS_H