#ifndef TPUNKT_CRYPTO_MANAGER_H
#define TPUNKT_CRYPTO_MANAGER_H

#include <cstddef>

namespace tpunkt
{
    // Holds state related to TPM - checks availability of TPM or device key and provides them
    struct CryptoManager
    {
        CryptoManager();
        ~CryptoManager();

        void encrypt(void* data, size_t len);
        void decrypt(void* data, size_t len);
    };

    CryptoManager& GetCryptoManager();

} // namespace tpunkt

#endif // TPUNKT_CRYPTO_MANAGER_H