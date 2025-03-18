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

// TODO implement rate limiting

// Methods return false if denied
struct EventLimiter final
{
    bool allowRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    //===== User =====//

    bool registerUserRequest(UserID user, EndpointClass eclass);
    void endUserRequest(UserID user, EndpointClass eclass);

    //===== Non-Users =====//

    bool registerRequest();
    void endRequest();

  private:
    ankerl::unordered_dense::map<uint64_t, IPRangeLimit> ipStatusMap;
    ankerl::unordered_dense::map<UserID, UserLimit> userStatusMap;
};

EventLimiter& GetEventLimiter();

} // namespace tpunkt


#endif // TPUNKT_WATCHER_H