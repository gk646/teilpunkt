#ifndef TPUNKT_AUTH_TOKEN_H
#define TPUNKT_AUTH_TOKEN_H

#include "fwd.h"

namespace tpunkt
{
    struct AuthToken final
    {
        const User& user;
    };

} // namespace tpunkt

#endif //TPUNKT_AUTH_TOKEN_H