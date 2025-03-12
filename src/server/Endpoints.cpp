// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "auth/Authenticator.h"
#include "auth/AuthToken.h"
#include "server/Endpoints.h"

namespace tpunkt
{

bool ServerEndpoint::AuthRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, UserID& user)
{
    for(const auto& [ key, value ] : *req)
    {
        if(key == TPUNKT_AUTH_SESSION_ID_NAME)
        {
            SessionID sessionId{value.data(), value.size()};
            SessionMetaData metaData;

            size_t userAgentLen = 0;
            const auto* userAgent = GetHeader(req, "user-agent", userAgentLen);
            if(userAgent == nullptr)
            {
                break;
            }
            metaData.userAgent = UserAgentString{userAgent, userAgentLen};

            const auto& ipAddr = res->getRemoteAddress();
            metaData.remoteAddress = HashedIP{ipAddr.data(), ipAddr.size()};
            // TODO hash ip

            AuthToken authToken;
            return GetAuthenticator().sessionAuth(sessionId, metaData, authToken) != AuthStatus::OK;
        }
    }

    res->writeStatus("401");
    res->end("Request has no session id");
    return false;
}

bool ServerEndpoint::HandleRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    return false;
}

void ServerEndpoint::RejectRequest(uWS::HttpResponse<true>* res, int code, const char* reason)
{
}

const char* ServerEndpoint::GetHeader(uWS::HttpRequest* req, const char* keyName, size_t& length)
{
    for(const auto& [ key, value ] : *req)
    {
        if(keyName == key)
        {
            length = value.size();
            return value.data();
        }
    }
    length = 0;
    return nullptr;
}

} // namespace tpunkt