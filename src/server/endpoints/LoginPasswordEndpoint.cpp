// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <glaze/json/read.hpp>
#include "auth/Authenticator.h"
#include "instance/InstanceConfig.h"
#include "server/DTO.h"
#include "server/DTOMappings.h"
#include "server/Endpoints.h"
#include "util/Strings.h"

namespace tpunkt
{

void LoginPasswordEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
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

            SecureWrapper<SessionToken> token;
            status = GetAuthenticator().sessionAdd(user, metaData, token);
            if(status != AuthStatus::OK)
            {
                EndRequest(res, 400, GetAuthStatusStr(status));
                return;
            }

            const uint32_t expiration = GetInstanceConfig().getNumber(NumberParamKey::USER_SESSION_EXPIRATION_SECS);
            auto reader = token.get();
            SetCookie(res, TPUNKT_AUTH_SESSION_ID_NAME, reader.get().c_str(), expiration);

            // Set user id cookie
            char numBuf[ 12 ];
            NumberToString(numBuf, 12, static_cast<uint32_t>(user));
            SetUnsafeCookie(res, TPUNKT_AUTH_SESSION_USER_NAME, numBuf, expiration);

            EndRequest(res, 200);
        });

    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt