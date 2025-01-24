#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "fwd.h"
#include "datastructures/FixedString.h"
#include "datastructures/SecureList.h"
#include "auth/Credentials.h"
#include "crypto/WrappedKey.h"

namespace tpunkt
{
    struct User final
    {
        UserName name;               // Unique name of the user
        Credentials credentials;     // Auth credentials
        SecureList<WrappedKey> keys; // Resource keys
        uint32_t userID;               // Abstract unique identifier

      private:
        TPUNKT_MACROS_STRUCT(User);
    };

} // namespace tpunkt

#endif // TPUNKT_USER_H