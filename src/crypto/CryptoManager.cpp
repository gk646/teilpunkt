#include <cstdlib>
#include "crypto/CryptoManager.h"
#include "util/Macros.h"
#include "util/Logging.h"

namespace tpunkt
{
    namespace global
    {
        static CryptoManager* CryptoManager;
    }

    CryptoManager::CryptoManager()
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN(CryptoManager);
    }

    CryptoManager::~CryptoManager()
    {
        TPUNKT_MACROS_GLOBAL_RESET(CryptoManager);
    }


    CryptoManager& GetCryptoManager()
    {
        TPUNKT_MACROS_GLOBAL_GET(CryptoManager);
    }

    void CryptoManager::encrypt(void* data, size_t len)
    {
        (void)data;
        (void)len;
    }

    void CryptoManager::decrypt(void* data, size_t len)
    {
        (void)data;
        (void)len;
    }

} // namespace tpunkt