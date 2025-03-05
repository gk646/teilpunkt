// SPDX-License-Identifier: GPL-3.0-only

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

        Session& operator=(Session&& other) noexcept
        {
            if(this != &other)
            {
                sessionID = std::move(other.sessionID);
                data = std::move(other.data);
                expiration = std::move(other.expiration);

                other.sessionID.clear();
                other.expiration.zero();
            }
            return *this;
        }


      private:
        SessionID sessionID;
        SessionMetaData data;
        const Timestamp creation{};
        Timestamp expiration{};
        friend SessionStorage;
    };

} // namespace tpunkt

#endif // TPUNKT_SESSION_H