#ifndef TPUNKT_SESSION_H
#define TPUNKT_SESSION_H

#include <cstdint>

#include "datastructures/FixedString.h"
#include "config.h"

using SessionID = tpunkt::FixedString<TPUNKT_CRYPTO_SESSION_LEN>;

// A session saves the authentication so the user does not have to authenticate on each request
// A session is terminated if ONE of the following is true:
//      - the current time is greater than the expiration stamp
//      - the remote address with the cookie differs from the saved one
//      - the user agent string differs from the saved one

struct Session final
{
    SessionID sessionID;
    tpunkt::FixedString<TPUNKT_STORAGE_NAME_LEN> userAgent;
    tpunkt::FixedString<20> remoteAddress;
    uint64_t expiration{};
};


#endif //TPUNKT_SESSION_H