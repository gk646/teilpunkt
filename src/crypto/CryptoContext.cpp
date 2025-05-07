// SPDX-License-Identifier: GPL-3.0-only

#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sodium.h>
#include <thread>
#include "crypto/CryptoContext.h"
#include "datastructures/FixedString.h"
#include "datastructures/Timestamp.h"
#include "instance/InstanceConfig.h"
#include "util/Logging.h"
#include "util/Macros.h"
#include "util/Strings.h"

namespace tpunkt
{
// otpauth://totp/MyApp:alice@example.com?secret=JBSWY3DP&issuer=Me&algorithm=SHA512

static_assert(32 == crypto_auth_hmacsha512_KEYBYTES, "Update key length");

namespace global
{
static CryptoContext* CryptoContext;
}


CryptoContext::CryptoContext()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(CryptoContext);
    TimedOneTimeKey key;
    printf("%s\n", getTOTPCreationString("Lukas", key).c_str());
    printf("%s\n", getTOTP(key).c_str());

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

void CryptoContext::encrypt(void* data, const size_t len)
{
    (void)data;
    (void)len;
}

void CryptoContext::decrypt(void* data, const size_t len)
{
    (void)data;
    (void)len;
}


TimedOneTimeCode CryptoContext::getTOTP(const TimedOneTimeKey& key) const
{
    unsigned char hmac[ crypto_auth_hmacsha512_BYTES ];
    constexpr int hmac_len = crypto_auth_hmacsha512_BYTES;

    uint64_t timeNum = static_cast<uint64_t>(time(nullptr));
    timeNum /= 30;                       // 30 second interval

    unsigned char timeBytes[ 8 + 1 ]{};
    for(int i = 7; i >= 0; --i)
    {
        timeBytes[ i ] = timeNum & 0xFF; // Only take the first byte
        timeNum >>= 8;                   // Discard the first byte
    }

    TimedOneTimeKey decodedKey;
    // Generates 20 bytes byte text
    Base32Decode(key.c_str(), 32, decodedKey.data(), decodedKey.capacity());
    crypto_auth_hmacsha512(hmac, timeBytes, 8, decodedKey.u_str());

    const uint64_t offset = (hmac[ hmac_len - 1 ] & 0x0F);
    uint64_t codeNum = (((hmac[ offset ] & 0x7F) << 24) | ((hmac[ offset + 1 ] & 0xFF) << 16) |
                        ((hmac[ offset + 2 ] & 0xFF) << 8) | ((hmac[ offset + 3 ] & 0xFF)));
    codeNum %= 1'000'000;

    TimedOneTimeCode code;
    (void)sprintf(code.data(), "%0*" PRIu64, 6, codeNum);
    return code;
}

FixedString<256> CryptoContext::getTOTPCreationString(const UserName& name, TimedOneTimeKey& out) const
{
    FixedString<256> result;
    crypto_auth_hmacsha512_keygen(out.udata());
    TimedOneTimeKey encodedKey;
    // Generates 32 bytes base32 text
    Base32Encode(out.data(), 20, encodedKey.data(), encodedKey.capacity());
    out = encodedKey;
    const char* fmt = "otpauth://totp/%s:%s?secret=%s&issuer=%s&algorithm=SHA512";
    const auto* instanceName = GetInstanceConfig().getString(StringParamKey::INSTANCE_NAME);
    (void)snprintf(result.data(), result.capacity(), fmt, "teilpunkt", name.c_str(), out.c_str(), instanceName);
    return result;
}

} // namespace tpunkt