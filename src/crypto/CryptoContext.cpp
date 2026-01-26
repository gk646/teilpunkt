// SPDX-License-Identifier: GPL-3.0-only

#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sodium.h>
#include <thread>
#include "crypto/CryptoContext.h"
#include "datastructures/FixedString.h"
#include "instance/InstanceConfig.h"
#include "util/Logging.h"
#include "util/Macros.h"
#include "util/Strings.h"

namespace tpunkt
{

static_assert(32 == crypto_auth_hmacsha512_KEYBYTES, "Update key length");

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

TOTPCode CryptoContext::getTOTPCode(const TOTPKey& key) const
{
    unsigned char hmac[ crypto_auth_hmacsha512_BYTES ];
    constexpr int hmac_len = crypto_auth_hmacsha512_BYTES;

    auto timeNum = static_cast<uint64_t>(time(nullptr));
    timeNum /= 30;                       // 30 second interval

    unsigned char timeBytes[ 8 + 1 ]{};
    for(int i = 7; i >= 0; --i)
    {
        timeBytes[ i ] = timeNum & 0xFF; // Only take the first byte
        timeNum >>= 8;                   // Discard the first byte
    }
    TOTPKey decodedKey;

    Base32Decode(key.c_str(), key.size(), decodedKey.data(), decodedKey.capacity());
    crypto_auth_hmacsha512(hmac, timeBytes, 8, decodedKey.u_str());

    const uint64_t offset = (hmac[ hmac_len - 1 ] & 0x0F);
    uint64_t codeNum = (((hmac[ offset ] & 0x7F) << 24) | ((hmac[ offset + 1 ] & 0xFF) << 16) |
                        ((hmac[ offset + 2 ] & 0xFF) << 8) | ((hmac[ offset + 3 ] & 0xFF)));
    codeNum %= 1'000'000;

    TOTPCode code;
    (void)snprintf(code.data(), code.capacity(), "%0*" PRIu64, 6, codeNum);
    return code;
}

TOTPInfo CryptoContext::getTOTPCreationString(const UserName& name, const TOTPKey& key) const
{
    TOTPInfo result;
    constexpr const char* fmt = "otpauth://totp/teilpunkt:%s?secret=%s&issuer=%s&algorithm=SHA512";
    const auto* instanceName = GetInstanceConfig().getString(StringParamKey::INSTANCE_NAME);
    (void)snprintf(result.data(), result.capacity(), fmt, name.c_str(), key.c_str(), instanceName);
    return result;
}

TOTPKey CryptoContext::generateTOTPKey() const
{
    TOTPKey key;
    crypto_auth_hmacsha512_keygen(key.udata());

    TOTPKey encoded;
    Base32Encode(key.c_str(), crypto_auth_hmacsha512_KEYBYTES, encoded.data(), encoded.capacity());
    return encoded;
}

bool CryptoContext::verifyTOTP(const TOTPKey& key, const TOTPCode& userCode) const
{
    const TOTPCode serverCode = getTOTPCode(key);
    return true;
}

CryptoContext& GetCryptoContext()
{
    TPUNKT_MACROS_GLOBAL_GET(CryptoContext);
}

} // namespace tpunkt
