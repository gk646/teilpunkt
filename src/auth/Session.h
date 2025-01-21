#ifndef TPUNKT_SESSION_H
#define TPUNKT_SESSION_H

#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"

namespace tpunkt
{
    struct SessionMetaData final
    {

      private:
        UserAgentString userAgent;
        HashedIP remoteAddress;
        friend SessionStorage;
    };

    // A session saves the authentication so the user does not have to authenticate on each request
    // A session is terminated if ONE of the following is true:
    //      - the current time is greater than the expiration stamp
    //      - the remote address with the cookie differs from the saved one
    //      - the user agent string differs from the saved one
    struct Session final
    {
        Session() = default;
        Session(const Session&) = default;

      private:
        SessionID sessionID;
        SessionMetaData data;
        const Timestamp creation{};
        Timestamp expiration{};
        friend SessionStorage;
    };

} // namespace tpunkt

#endif // TPUNKT_SESSION_H