// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <glaze/json/read.hpp>
#include "auth/Authenticator.h"
#include "server/DTO.h"
#include "server/DTOMappings.h"
#include "server/Endpoints.h"

namespace tpunkt
{

void AuthPasswordEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    thread_local std::vector<DTOSessionInfo> sessionCollector;

    if(!AllowRequest(res, req))
    {
        return;
    }

    res->onData(
        [ res, req ](std::string_view data, const bool last)
        {
            if(!last)
            {
                EndRequest(res, 431, "Sent data too large");
                return;
            }

            DTOUserLoginPW authData{};
            const auto error = glz::read_json(authData, data);
            if(error)
            {
                EndRequest(res, 400, "Bad JSON");
                return;
            }

            UserID user{};
            Credentials credentials{};
            credentials.type = CredentialsType::PASSWORD;
            credentials.password = authData.password;
            auto status = GetAuthenticator().userLogin(authData.name, credentials, user);
            if(status != AuthStatus::OK)
            {
                EndRequest(res, 400, GetAuthStatusStr(status));
                return;
            }

            SessionMetaData metaData{};
            if(!GetMetaData(res, req, metaData))
            {
                EndRequest(res, 400, "Missing headers");
                return;
            }

            status = GetAuthenticator().sessionGetInfo(user, sessionCollector);
            if(status != AuthStatus::OK)
            {
                EndRequest(res, 500);
                return;
            }




            res->writeHeader("Set-Cookie", );
            EndRequest(res, 200);
        });

    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt