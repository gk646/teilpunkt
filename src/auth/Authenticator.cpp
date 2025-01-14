#include <cstdlib>
#include <sodium.h>
#include "auth/Authenticator.h"
#include "auth/SessionStorage.h"
#include "auth/AuthToken.h"
#include "util/Memory.h"
#include "datastructures/SecureList.h"
#include "common/User.h"

namespace tpunkt
{
    namespace global
    {
        static Authenticator* Authenticator;
    }

    Authenticator::Authenticator()
    {
        sessionStore = TPUNKT_ALLOC( sessionStore, sizeof( int* ) );
        userStore = TPUNKT_ALLOC( userStore, sizeof( int* ) );
        TPUNKT_MACROS_GLOBAL_ASSIGN( Authenticator );
    }

    Authenticator::~Authenticator()
    {
        TPUNKT_FREE( sessionStore );
        TPUNKT_FREE( userStore );
        TPUNKT_MACROS_GLOBAL_RESET( Authenticator );
    }

    Authenticator& GetAuthenticator()
    {
        TPUNKT_MACROS_GLOBAL_GET( Authenticator );
    }


    AuthToken Authenticator::loginUser( const UserName& name, const Credentials& credentials )
    {
        User user{};
        return AuthToken{ user, randombytes_random() };
    }

    AuthToken Authenticator::authCookie( const SessionID& sessionId )
    {
    }

    bool Authenticator::isValid( const AuthToken& token )
    {
    }


} // namespace tpunkt