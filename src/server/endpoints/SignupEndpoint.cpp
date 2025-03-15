// SPDX-License-Identifier: GPL-3.0-only

#include <App.h>
#include "auth/Authenticator.h"
#include "server/DTO.h"
#include "server/DTOMappings.h"
#include "server/Endpoints.h"

namespace tpunkt
{

void SignupEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(HandleRequest(res, req))
    {
        return;
    }

    res->onData(
        [ res ](std::string_view data, const bool last)
        {
            if(!last)
            {
                return EndRequest(res, 431, "Sent data too large");
            }

            DTOUserSignup signupData{};
            const auto error = glz::read_json(signupData, data);
            if(error)
            {
                return EndRequest(res, 400, uWS::httpErrorResponses[ uWS::HTTP_ERROR_400_BAD_REQUEST ].data());
            }
            const auto status = GetAuthenticator().userAdd(signupData.name, signupData.credentials);
            if(status != AuthStatus::OK)
            {
                return EndRequest(res, 400, GetAuthStatusStr(status));
            }
            res->writeStatus(uWS::HTTP_200_OK);
            res->end();
        });

    res->onAborted([ & ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt