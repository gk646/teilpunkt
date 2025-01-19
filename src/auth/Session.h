#ifndef TPUNKT_SESSION_H
#define TPUNKT_SESSION_H

namespace tpunkt
{

    struct SessionData final
    {
        UserAgentString userAgent;
        HashedIP remoteAddress;
    };

    // A session saves the authentication so the user does not have to authenticate on each request
    // A session is terminated if ONE of the following is true:
    //      - the current time is greater than the expiration stamp
    //      - the remote address with the cookie differs from the saved one
    //      - the user agent string differs from the saved one
    struct Session final
    {
        Session() = default;
        SessionID sessionID;
        SessionData data;
        uint64_t creation; // Unix timestamps
        uint64_t expiration;
    };

} // namespace tpunkt

#endif // TPUNKT_SESSION_H