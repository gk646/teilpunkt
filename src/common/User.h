// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_USER_H
#define TPUNKT_USER_H

#include "auth/Credentials.h"
#include "crypto/WrappedKey.h"
#include "datastructures/FixedString.h"
#include "datastructures/SecureList.h"
#include "fwd.h"

namespace tpunkt
{

struct User final
{
    UserName name;               // Unique name of the user
    Credentials credentials;     // Auth credentials
    SecureList<WrappedKey> keys; // Resource keys
    UserID userID;               // Abstract unique identifier

  private:
    TPUNKT_MACROS_STRUCT(User);
};

} // namespace tpunkt

#endif // TPUNKT_USER_H