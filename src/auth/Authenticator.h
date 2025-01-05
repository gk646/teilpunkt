#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include <string>

struct Authenticator final
{
    bool authPassword(std::string& user, std::string& password);
};


Authenticator& GetAuthenticator();

#endif //TPUNKT_AUTHENTICATOR_H