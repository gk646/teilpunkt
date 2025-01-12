#ifndef TPUNKT_MACROS_H
#define TPUNKT_MACROS_H


#define TPUNKT_MACROS_DEL_CTORS(type)                                                                                   \
    type(type&&) = delete;                                                                                              \
    type(const type&) = delete;                                                                                         \
    type& operator=(const type&) = delete;                                                                              \
    type& operator=(const type&&) = delete;

#define TPUNKT_MACROS_STRUCT(type) TPUNKT_MACROS_DEL_CTORS(type)


#endif //TPUNKT_MACROS_H