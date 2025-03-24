// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <glaze/json/read.hpp>
#include "auth/Authenticator.h"
#include "server/DTO.h"
#include "server/Endpoints.h"
#include "server/DTOMappings.h"

namespace tpunkt
{

void AuthEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(AllowRequest(res, req))
    {
        return;
    }

    res->onData(
        [ res ](std::string_view data, const bool last)
        {
            if(!last)
            {
                EndRequest(res, 431, "Sent data too large");
                return;
            }

            DTOUserLogin authData{};
            const auto error = glz::read_json(authData, data);
            if(error)
            {
                EndRequest(res, 400, "Bad JSON");
                return;
            }

            UserID user{};
            const auto status = GetAuthenticator().userLogin(authData.name, authData.credentials, user);
            if(status != AuthStatus::OK)
            {
                EndRequest(res, 400, GetAuthStatusStr(status));
                return;
            }
            EndRequest(res, 200);
        });

    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt