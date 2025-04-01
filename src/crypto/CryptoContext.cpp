// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "crypto/CryptoContext.h"
#include "datastructures/FixedString.h"
#include "util/Logging.h"
#include "util/Macros.h"

namespace tpunkt
{

namespace global
{
static CryptoContext* CryptoContext;
}

CryptoContext::CryptoContext()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(CryptoContext);

    // TODO init tpm or ephemeral key
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