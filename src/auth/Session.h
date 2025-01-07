#ifndef TPUNKT_SESSION_H
#define TPUNKT_SESSION_H

#include <cstdint>

#include "datastructures/FixedString.h"
#include "config.h"


struct Session final
{
    tpunkt::FixedString<TPUNKT_SESSION_ID_BYTES> sessionID;

    // user agent string
    uint64_t expiration; // Only save expiration - not when it was issued
};

#endif //TPUNKT_SESSION_H