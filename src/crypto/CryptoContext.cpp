// SPDX-License-Identifier: GPL-3.0-only

#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sodium.h>
#include <thread>
#include <unistd.h>
#include "crypto/CryptoContext.h"
#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"
#include "util/Logging.h"
#include "util/Macros.h"

namespace tpunkt
{
// otpauth://totp/MyApp:alice@example.com?secret=JBSWY3DP&issuer=Me&algorithm=SHA512

namespace global
{
static CryptoContext* CryptoContext;
}


bool otp_num_to_bytestring(uint64_t integer, char* out_str)
{
    if(out_str == NULL)
        return false;

    size_t i = 7;
    while(integer != 0)
    {
        out_str[ i ] = integer & 0xFF;
        i--;
        integer >>= 8;
    }
    return true;
}

CryptoContext::CryptoContext()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(CryptoContext);

    while(true)
    {
        unsigned char hmac[ crypto_auth_hmacsha512_BYTES ];
        constexpr int hmac_len = crypto_auth_hmacsha512_BYTES;

        FixedString<crypto_auth_hmacsha512_KEYBYTES> key = "Hello";
        auto now = Timestamp::Now();
        // crypto_auth_hmacsha512_keygen(key);

        uint64_t seconds = (uint64_t)time(NULL);
        seconds /= 30;

        char byte_string[ 8 + 1 ];
        memset(byte_string, 0, 8 + 1);

        if(!otp_num_to_bytestring(seconds, byte_string))
        {
            return;
        }

        crypto_auth_hmacsha512(hmac, (const unsigned char*)byte_string, 8, key.u_str());

        uint64_t offset = (hmac[ hmac_len - 1 ] & 0x0F);
        uint64_t code = (((hmac[ offset ] & 0x7F) << 24) | ((hmac[ offset + 1 ] & 0xFF) << 16) |
                         ((hmac[ offset + 2 ] & 0xFF) << 8) | ((hmac[ offset + 3 ] & 0xFF)));

        code %= 1'000'000;

        char buff[ 32 ];
        sprintf(buff, "%0*" PRIu64, 6, code);
        puts(buff);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }


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