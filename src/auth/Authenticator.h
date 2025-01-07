#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include <string>

#include "auth/SessionStorage.h"

namespace tpunkt
{
    struct Authenticator final
    {
        bool authPassword(std::string& user, std::string& password);

        bool authCookie(std::string& sessionID);


    };

    Authenticator& GetAuthenticator();
} // namespace tpunkt

#endif //TPUNKT_AUTHENTICATOR_H