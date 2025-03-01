// SPDX-License-Identifier: Apache License 2.0

#include <cstdlib>
#include "crypto/CryptoContext.h"
#include "util/Macros.h"
#include "util/Logging.h"
#include <unistd.h>

namespace tpunkt
{
    namespace global
    {
        static CryptoContext* CryptoContext;
    }

    CryptoContext::CryptoContext()
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN(CryptoContext);

            char buffer[TPUNKT_INSTANCE_SECRET_MAX_LEN];
            fputs("Input the instance secret:",stdout);
            ssize_t bytesRead = read(STDIN_FILENO, buffer, TPUNKT_INSTANCE_SECRET_MAX_LEN - 1);

            if(bytesRead == -1 || bytesRead == 0){
                LOG_FATAL("Failed to read instance secret");
            }
        buffer[bytesRead - 1] = '\0';



    }

    CryptoContext::~CryptoContext()
    {
        TPUNKT_MACROS_GLOBAL_RESET(CryptoContext);
    }


    CryptoContext& GetCryptoContext()
    {
        TPUNKT_MACROS_GLOBAL_GET(CryptoContext);
    }

    void CryptoContext::encrypt(void* data, size_t len)
    {
        (void)data;
        (void)len;
    }

    void CryptoContext::decrypt(void* data, size_t len)
    {
        (void)data;
        (void)len;
    }

} // namespace tpunkt