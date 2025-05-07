// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_CRYPTO_CONTEXT_H
#define TPUNKT_CRYPTO_CONTEXT_H

#include "datastructures/FixedString.h"
#include "util/Macros.h"

namespace tpunkt
{

// Holds state related to TPM - checks availability of TPM or device key and provides them
struct CryptoContext
{
    CryptoContext();
    ~CryptoContext();

    void encrypt(void* data, size_t len);
    void decrypt(void* data, size_t len);

    [[nodiscard]] TimedOneTimeCode getTOTP(const TimedOneTimeKey& key) const;

    [[nodiscard]] FixedString<256> getTOTPCreationString(const UserName& name, TimedOneTimeKey& out) const;

  private:
    TPUNKT_MACROS_STRUCT(CryptoContext);
};

CryptoContext& GetCryptoContext();

} // namespace tpunkt

#endif // TPUNKT_CRYPTO_CONTEXT_H