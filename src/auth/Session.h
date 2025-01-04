#ifndef TPUNKT_SESSION_H
#define TPUNKT_SESSION_H

#include <cstdint>
#include "config.h"

struct Session final
{
    char sessionID[TPUNKT_SESSION_ID_BYTES]{};
    // user agent string
    uint64_t expiration; // Only save expiration - not when it was issued
};

#endif //TPUNKT_SESSION_H