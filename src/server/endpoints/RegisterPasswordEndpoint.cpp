// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "auth/Authenticator.h"
#include "server/DTO.h"
#include "server/DTOMappings.h"
#include "server/Endpoints.h"

namespace tpunkt
{

void RegisterPasswordEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(IsRateLimited(res, req))
    {
        return;
    }

    const auto handlerFunc = [ res ](std::string_view data, const bool last)
    {
        if(!last)
        {
            EndRequest(res, 431, "Sent data too large");
            return;
        }

        DTO::RequestUserSignupPassword signupData{};
        const auto error = glz::read_json(signupData, data);
        if(error)
        {
            EndRequest(res, 400, "Sent Bad JSON");
            return;
        }

        Credentials credentials;
        credentials.type = CredentialsType::PASSWORD;
        credentials.password = signupData.password;

        const auto status = Authenticator::GetInstance().userAdd(UserID::SERVER, signupData.name, credentials);
        if(status != AuthStatus::OK)
        {
            EndRequest(res, 400, Authenticator::GetStatusStr(status));
            return;
        }

        const TOTPInfo info = GetCryptoContext().getTOTPCreationString(signupData.name, credentials.totpKey);
        EndRequest(res, 200, info.view());
    };

    res->onData(handlerFunc);
    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt
