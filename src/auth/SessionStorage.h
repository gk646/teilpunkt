#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "fwd.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{

    // A session saves the authentication so the user does not have to authenticate on each request
    // A session is terminated if ONE of the following is true:
    //      - the current time is greater than the expiration stamp
    //      - the remote address with the cookie differs from the saved one
    //      - the user agent string differs from the saved one
    struct Session final
    {
        SessionID sessionID;
        FixedString<TPUNKT_STORAGE_NAME_LEN> userAgent;
        FixedString<20> remoteAddress;
        uint64_t expiration{};
    };

    struct SessionStorage final
    {
        std::vector<Session> sessions;
        SessionID addSession(const User& user);
    };

} // namespace tpunkt

#endif //TPUNKT_SESSION_STORAGE_H