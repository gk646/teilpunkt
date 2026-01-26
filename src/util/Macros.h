// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_MACROS_H
#define TPUNKT_MACROS_H

#define TPUNKT_MACROS_DEL_CTORS(type)                                                                                  \
    type(type&&) = delete;                                                                                             \
    type(const type&) = delete;                                                                                        \
    type& operator=(const type&) = delete;                                                                             \
    type& operator=(type&&) = delete

#define TPUNKT_MACROS_STRUCT(type) TPUNKT_MACROS_DEL_CTORS(type)

#define TPUNKT_MACROS_MOVE_ONLY(type)                                                                                  \
    type(const type&) = delete;                                                                                        \
    type& operator=(const type&) = delete;                                                                             \
    type(type&&) noexcept = default;                                                                                   \
    type& operator=(type&&) noexcept = default


#define TPUNKT_MACROS_GLOBAL_ASSIGN(var)                                                                               \
    if(global::var == nullptr)                                                                                         \
    {                                                                                                                  \
        global::var = this;                                                                                            \
        LOG_DEBUG("%-15s initialized", #var);                                                                          \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        LOG_CRITICAL("%-15s is not assigned", #var);                                                                   \
    }

#define TPUNKT_MACROS_GLOBAL_RESET(var)                                                                                \
    if(global::var != nullptr)                                                                                         \
    {                                                                                                                  \
        LOG_DEBUG("%-15s stopped", #var);                                                                              \
        global::var = nullptr;                                                                                         \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        LOG_CRITICAL("%-15s is not assigned", #var);                                                                   \
    }

#define TPUNKT_MACROS_GLOBAL_GET(var)                                                                                  \
    if(global::var != nullptr)                                                                                         \
    {                                                                                                                  \
        return *global::var;                                                                                           \
    }                                                                                                                  \
    LOG_FATAL("%-15s is not assigned", #var);


#define TPUNKT_MACROS_STARTUP_PRINT()                                                                                  \
    constexpr auto STARTUP_MSG = R"(                ,,    ,,                                                     
  mm            db  `7MM                                   `7MM        mm    
  MM                  MM                                     MM        MM    
mmMMmm .gP"Ya `7MM    MM `7MMpdMAo.`7MM  `7MM  `7MMpMMMb.    MM  ,MP'mmMMmm  
  MM  ,M'   Yb  MM    MM   MM   `Wb  MM    MM    MM    MM    MM ;Y     MM    
  MM  8M""""""  MM    MM   MM    M8  MM    MM    MM    MM    MM;Mm     MM    
  MM  YM.    ,  MM    MM   MM   ,AP  MM    MM    MM    MM    MM `Mb.   MM    
  `Mbmo`Mbmmd'.JMML..JMML. MMbmmd'   `Mbod"YML..JMML  JMML..JMML. YA.  `Mbmo 
                           MM                                                
                         .JMML.                                              
teilpunkt™ %s Built %s
Copyright (C) 2025 gk646 and contributors
This is free software; see the license for copying conditions. There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

)";                                                                                                                    \
    constexpr auto BUILD_TIMESTAMP = __DATE__ " " __TIME__;                                                            \
    (void)fprintf(stdout, STARTUP_MSG, TPUNKT_VERSION, BUILD_TIMESTAMP)


#define TPUNKT_MACROS_AUTH_USER()                                                                                      \
    if(IsRateLimited(res, req))                                                                                        \
    {                                                                                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    UserID user{};                                                                                                     \
    if(!HasValidSession(res, req, user))                                                                               \
    {                                                                                                                  \
        return;                                                                                                        \
    }


#endif // TPUNKT_MACROS_H
