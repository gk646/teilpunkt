// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_CRYPTO_MANAGER_H
#define TPUNKT_CRYPTO_MANAGER_H

#include <cstddef>
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

      private:
        TPUNKT_MACROS_STRUCT(CryptoContext);
    };

    CryptoContext& CryptoContext();

} // namespace tpunkt

#endif // TPUNKT_CRYPTO_MANAGER_H