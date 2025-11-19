// SPDX-License-Identifier: GPL-3.0-only

#include <sodium/crypto_secretbox.h>
#include <sodium/randombytes.h>
#include "crypto/Ciphers.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{

BlockCipher::BlockCipher(const CipherKey& key) : key(key)
{
}

size_t BlockCipher::GetEncryptMinLen(const size_t ilen)
{
    return crypto_secretbox_MACBYTES + ilen + crypto_secretbox_NONCEBYTES;
}

bool BlockCipher::encrypt(const unsigned char* input, const size_t ilen, unsigned char* out, const size_t olen) const
{
    if(olen < GetEncryptMinLen(ilen)) [[unlikely]] // Saves nonce as well
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

bool BlockCipher::decrypt(const unsigned char* input, const size_t ilen, unsigned char* out, const size_t olen) const
{
    // Nonce and mac not written to output
    if(olen < (ilen - crypto_secretbox_MACBYTES - crypto_secretbox_NONCEBYTES)) [[unlikely]]
    {
        return false;
    }
    const auto inputLen = ilen - crypto_secretbox_NONCEBYTES;
    // First bytes are the nonce
    return crypto_secretbox_open_easy(out, input + crypto_secretbox_NONCEBYTES, inputLen, input, key.u_str()) == 0;
}

StreamCipherEncrypt::StreamCipherEncrypt(const CipherKey& key, const size_t chunkSize, unsigned char* out,
                                         const size_t olen)
    : out(out), olen(olen), chunkSize(chunkSize)
{
    if(olen < crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
        LOG_FATAL("Output too small");
    }
    (void)crypto_secretstream_xchacha20poly1305_init_push(&state, header, key.u_str());
    memcpy(out, header, crypto_secretstream_xchacha20poly1305_HEADERBYTES);
    offset += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
}

size_t StreamCipherEncrypt::GetMinBufSize(const size_t ilen, const size_t chunks)
{
    // Header applied to first one
    return chunks * crypto_secretstream_xchacha20poly1305_ABYTES + ilen +
           crypto_secretstream_xchacha20poly1305_HEADERBYTES;
}

bool StreamCipherEncrypt::encrypt(const unsigned char* input, const size_t ilen, const bool isLast)
{
    if(offset + ilen + crypto_secretstream_xchacha20poly1305_ABYTES > olen) [[unlikely]]
    {
        return false;
    }

    if(ilen != chunkSize) [[unlikely]]
    {
        return false;
    }

    unsigned long long written = 0;
    const unsigned char tag = isLast ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
    unsigned char* outPtr = out + offset;
    if(crypto_secretstream_xchacha20poly1305_push(&state, outPtr, &written, input, ilen, nullptr, 0, tag) != 0)
        [[unlikely]]
    {
        return false;
    }

    if(written != chunkSize + crypto_secretstream_xchacha20poly1305_ABYTES) [[unlikely]]
    {
        return false;
    }

    offset += written;
    return true;
}

size_t StreamCipherEncrypt::getChunkSize() const
{
    return chunkSize;
}

size_t StreamCipherEncrypt::getWritten() const
{
    return offset;
}


StreamCipherDecrypt::StreamCipherDecrypt(const CipherKey& key, const size_t chunkSize, const unsigned char* input,
                                         const size_t ilen)
    : input(input), chunkSize(chunkSize), ilen(ilen)
{
    if(ilen < crypto_secretstream_xchacha20poly1305_HEADERBYTES) [[unlikely]]
    {
        LOG_FATAL("Input too small");
    }
    state = {};
    if(crypto_secretstream_xchacha20poly1305_init_pull(&state, input, key.u_str()) != 0)
    {
        LOG_ERROR("Incomplete header");
        return; // incomplete header
    }
    offset += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
}

size_t StreamCipherDecrypt::GetMinBufSize(const size_t ilen, const size_t chunks)
{
    return ilen - (crypto_secretstream_xchacha20poly1305_ABYTES * chunks);
}

bool StreamCipherDecrypt::decrypt(unsigned char* out, const size_t olen)
{
    if(chunkSize > olen) [[unlikely]]
    {
        return false;
    }

    unsigned char tag = 0;
    unsigned long long written = 0;
    const unsigned char* inputPtr = input + offset;
    const unsigned long long inSize = chunkSize + crypto_secretstream_xchacha20poly1305_ABYTES;

    if(crypto_secretstream_xchacha20poly1305_pull(&state, out, &written, &tag, inputPtr, inSize, nullptr, 0) != 0)
    {
        return false; // Corrupted chunk
    }

    if(written != chunkSize) [[unlikely]]
    {
        return false;
    }

    offset += inSize;
    const bool isLast = offset == ilen;

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

size_t StreamCipherDecrypt::getChunkSize() const
{
    return chunkSize;
}

} // namespace tpunkt
