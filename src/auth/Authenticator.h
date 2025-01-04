#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>

struct Authenticator final
{
    bool authPassword(std::string& user, std::string& password);
};


#endif //AUTHENTICATOR_H