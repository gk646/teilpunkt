// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "monitoring/EventLimiter.h"
#include "server/Endpoints.h"
#include "util/Logging.h"
#include "util/Macros.h"

namespace tpunkt
{

namespace global
{
EventLimiter* EventLimiter;
}

EventLimiter::EventLimiter()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(EventLimiter);
}

EventLimiter::~EventLimiter()
{
    TPUNKT_MACROS_GLOBAL_RESET(EventLimiter);
}

EventLimiter& GetEventLimiter()
{
    TPUNKT_MACROS_GLOBAL_GET(EventLimiter);
}

bool EventLimiter::allowRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    const std::string_view ipBinary = res->getRemoteAddress();
    uint32_t ipNum = *(uint32_t*)ipBinary.data();
    ipNum &= 0xFFFFFF00; // Zero out last 8 bits (unify to /24 subnet)

    IPRangeLimit& ipStatus = ipStatusMap[ ipNum ];
    const Timestamp now = Timestamp::Now();
    const Timestamp diff = now - ipStatus.last;

    const int32_t addTokens = IPRangeLimit::TOKENS_PER_SEC * static_cast<int32_t>(diff.getSeconds());
    ipStatus.tokens = std::min(IPRangeLimit::TOKENS_MAX, ipStatus.tokens + addTokens);

    ipStatus.last = now;
    ipStatus.tokens -= IPRangeLimit::TOKEN_COST_CONN;

    if(ipStatus.tokens < 0)
    {
        ++ipStatus.overLimitRequests;
        if(ipStatus.overLimitRequests > IPRangeLimit::OVER_LIMIT_LIMIT)
        {
            ipStatus.overLimitRequests = 0;
            ipStatus.tokens = IPRangeLimit::TOKEN_MAX_MINUS;
        }
        ServerEndpoint::EndRequest(res, 429, "", true);
        return false;
    }
    return true;
}

bool EventLimiter::startUserRequest(UserID user, EndpointClass eclass)
{
}

void EventLimiter::endUserRequest(UserID user, EndpointClass eclass)
{
}

bool EventLimiter::startRequest()
{
}

void EventLimiter::endRequest()
{
}


} // namespace tpunkt
