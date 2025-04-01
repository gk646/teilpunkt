// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_WATCHER_H
#define TPUNKT_WATCHER_H

#include <cstdint>
#include <ankerl/unordered_dense.h>
#include "fwd.h"
#include "monitoring/Limits.h"

namespace tpunkt
{

enum class EndpointClass : uint8_t
{
    STATIC,
    UPLOAD,
    DOWNLOAD,
    USER_AUTH,
    USER_SIGNUP,

};

// Methods return false if denied
struct EventLimiter final
{

    EventLimiter();
    ~EventLimiter();

    // Returns true if the request is allowed
    bool allowRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    //===== User =====//

    // Returns true if the user can
    bool startUserRequest(UserID user, EndpointClass eclass);
    void endUserRequest(UserID user, EndpointClass eclass);

    //===== Non-Users =====//

    bool startRequest();
    void endRequest();

  private:
    ankerl::unordered_dense::map<uint64_t, IPRangeLimit> ipStatusMap;
    ankerl::unordered_dense::map<UserID, UserLimit> userStatusMap;
};

EventLimiter& GetEventLimiter();

} // namespace tpunkt


#endif // TPUNKT_WATCHER_H