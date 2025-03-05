// SPDX-License-Identifier: GPL-3.0-only

#include <sodium/crypto_secretbox.h>
#include <sodium/randombytes.h>
#include "crypto/Ciphers.h"
#include "datastructures/FixedString.h"

#if (TPUNKT_CRYPTO_KEY_LEN != crypto_secretstream_xchacha20poly1305_KEYBYTES) ||                                       \
    (TPUNKT_CRYPTO_KEY_LEN != crypto_secretbox_KEYBYTES)
#error "Update length ?"
#endif

namespace tpunkt
{

BlockCipher::BlockCipher(const CipherKey& key) : key(key)
{
}

size_t BlockCipher::getEncryptMinLen(const size_t ilen)
{
    return crypto_secretbox_MACBYTES + ilen + crypto_secretbox_NONCEBYTES;
}

bool BlockCipher::encrypt(const unsigned char* input, const size_t ilen, unsigned char* out, const size_t olen)
{
    if(olen < getEncryptMinLen(ilen)) [[unlikely]] // Saves nonce as well
    {
        return false;
    }

    unsigned char nonce[ crypto_secretbox_NONCEBYTES ];
    randombytes_buf(nonce, crypto_secretbox_NONCEBYTES);

    // Nonce at first position
    memcpy(out, nonce, crypto_secretbox_NONCEBYTES);
    out = out + crypto_secretbox_NONCEBYTES;
    return crypto_secretbox_easy(out, input, ilen, nonce, key.u_str()) == 0;
}


bool BlockCipher::decrypt(const unsigned char* input, const size_t ilen, unsigned char* out, const size_t olen)
{
    if(olen < ilen) [[unlikely]] // Nonce and mac not written to output
    {
        return false;
    }

    // First bytes are the nonce
    return crypto_secretbox_open_easy(out, input + crypto_secretbox_NONCEBYTES, ilen, input, key.u_str()) == 0;
}

} // namespace tpunkt


tpunkt::StreamCipher::StreamCipher(const CipherKey& key) : key(key)
{
}

size_t tpunkt::StreamCipher::getEncryptMinLen(const size_t ilen)
{
    // Header applied to first one
    return ilen + crypto_secretstream_xchacha20poly1305_ABYTES + crypto_secretstream_xchacha20poly1305_HEADERBYTES;
}

bool tpunkt::StreamCipher::encrypt(const unsigned char* input, const size_t ilen, unsigned char* out, size_t olen,
                                   const bool isLast)
{

    if(olen < getEncryptMinLen(ilen)) [[unlikely]]
    {
        return false;
    }

    if(!headUsed) [[unlikely]]
    {
        (void)crypto_secretstream_xchacha20poly1305_init_push(&state, header, key.u_str());
        if(olen > crypto_secretstream_xchacha20poly1305_HEADERBYTES)
        {
            memcpy(out, header, crypto_secretstream_xchacha20poly1305_HEADERBYTES);
            out = out + crypto_secretstream_xchacha20poly1305_HEADERBYTES;
            olen -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
            headUsed = true;
        }
        else
        {
            return false;
        }
    }

    const auto writeSize = ilen + crypto_secretstream_xchacha20poly1305_ABYTES;
    unsigned long long written = 0;
    const unsigned char tag = isLast ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
    if(crypto_secretstream_xchacha20poly1305_push(&state, out, &written, input, ilen, nullptr, 0, tag) != 0)
        [[unlikely]]
    {
        return false;
    }

    if(written != writeSize) [[unlikely]]
    {
        return false;
    }

    return true;
}

bool tpunkt::StreamCipher::decrypt(const unsigned char* input, const size_t ilen, unsigned char* out, const size_t olen,
                                   const bool isFirst, const bool isLast)
{
    if(isFirst) [[unlikely]]
    {
        state = {};
        if(crypto_secretstream_xchacha20poly1305_init_pull(&state, header, key.u_str()) != 0)
        {
            return false; // incomplete header
        }
    }

    if(olen < ilen)
    {
        return false;
    }

    unsigned char tag;
    unsigned long long written = 0;
    if(crypto_secretstream_xchacha20poly1305_pull(&state, out, &written, &tag, input, ilen, nullptr, 0) != 0)
    {
        return false;     // Corrupted chunk
    }

    if(tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL) [[unlikely]]
    {
        if(!isLast)
        {
            return false; // end of stream reached before the end of the file
        }
    }
    else
    {
        if(isLast)
        {
            return false; // end of file reached before the end of the stream
        }
    }

    return true;
}