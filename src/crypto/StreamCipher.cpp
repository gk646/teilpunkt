// SPDX-License-Identifier: Apache License 2.0

#include "crypto/StreamCipher.h"
#include "datastructures/FixedString.h"

#if TPUNKT_CRYPTO_KEY_LEN != crypto_secretstream_xchacha20poly1305_KEYBYTES
#error "Update length ?"
#endif

tpunkt::StreamCipher::StreamCipher(const CipherKey& key)
{
    (void)crypto_secretstream_xchacha20poly1305_init_push(&st, header, (const unsigned char*)key.c_str());
}

bool tpunkt::StreamCipher::encrypt(const unsigned char* input, const size_t inLen, unsigned char* out, size_t outLen,
                                   const bool isLast)
{
    unsigned long long written = 0;

    if(!headUsed) [[unlikely]]
    {
        if(outLen > crypto_secretstream_xchacha20poly1305_HEADERBYTES)
        {
            memcpy(out, header, crypto_secretstream_xchacha20poly1305_HEADERBYTES);
            out = out + crypto_secretstream_xchacha20poly1305_HEADERBYTES;
            outLen -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
        }
        else
        {
            return false;
        }
    }

    const auto writeSize = inLen + crypto_secretstream_xchacha20poly1305_ABYTES;
    if(outLen < writeSize) [[unlikely]]
    {
        return false;
    }

    const unsigned char tag = isLast ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
    if(crypto_secretstream_xchacha20poly1305_push(&st, out, &written, input, inLen, nullptr, 0, tag) != 0) [[unlikely]]
    {
        return false;
    }

    if(written != writeSize) [[unlikely]]
    {
        return false;
    }

    return true;
}

bool tpunkt::StreamCipher::decrypt(const unsigned char* input, size_t inLen, unsigned char* out, size_t outLen,
                                   bool isLast)
{
}