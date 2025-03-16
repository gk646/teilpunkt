// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_SESSION_H
#define TPUNKT_SESSION_H

#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"

namespace tpunkt
{

struct SessionMetaData final
{
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

    Session(Session&& other) noexcept
        : sessionID(other.sessionID), metaData(other.metaData), creation(other.creation), expiration(other.expiration)
    {
    }

    bool isExpired() const;

  private:
    int num;
    SessionID sessionID;
    SessionMetaData metaData;
    const Timestamp creation;
    const Timestamp expiration;
    friend SessionStorage;
};

} // namespace tpunkt

#endif // TPUNKT_SESSION_H