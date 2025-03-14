// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_STREAM_CIPHER_H
#define TPUNKT_STREAM_CIPHER_H

#include <sodium/crypto_secretstream_xchacha20poly1305.h>
#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{

struct BlockCipher final
{
    explicit BlockCipher(const CipherKey& key);

    static size_t GetEncryptMinLen(size_t ilen);

    bool encrypt(const unsigned char* input, size_t ilen, unsigned char* out, size_t olen) const;
    bool decrypt(const unsigned char* input, size_t ilen, unsigned char* out, size_t olen) const;

  private:
    CipherKey key;
};

struct StreamCipherEncrypt final
{
    // out buffer to write to and its total length
    explicit StreamCipherEncrypt(const CipherKey& key, size_t chunkSize, unsigned char* out, size_t olen);

    // Size needed to hold entire input encrypted with the given amount of chunks
    static size_t GetMinBufSize(size_t ilen, size_t chunks);

    // Encrypts the given chunk and appends to out - must be of length chunkSize
    bool encrypt(const unsigned char* input, size_t ilen, bool isLast);

    [[nodiscard]] size_t getChunkSize() const;

    // Returns total bytes written to out
    [[nodiscard]] size_t getWritten() const;

  private:
    crypto_secretstream_xchacha20poly1305_state state{};
    unsigned char header[ crypto_secretstream_xchacha20poly1305_HEADERBYTES ]{};
    unsigned char* out = nullptr;
    const size_t olen = 0;
    const size_t chunkSize = 0;
    size_t offset = 0;
};

struct StreamCipherDecrypt final
{
    explicit StreamCipherDecrypt(const CipherKey& key, size_t chunkSize, const unsigned char* input, size_t ilen);

    static size_t GetMinBufSize(size_t ilen, size_t chunks);

    bool decrypt(unsigned char* out, size_t olen);

    [[nodiscard]] size_t getChunkSize() const;

  private:
    crypto_secretstream_xchacha20poly1305_state state{};
    unsigned char header[ crypto_secretstream_xchacha20poly1305_HEADERBYTES ]{};
    const unsigned char* input = nullptr;
    const size_t chunkSize = 0;
    const size_t ilen = 0;
    size_t offset = 0;
};

} // namespace tpunkt

#endif // TPUNKT_STREAM_CIPHER_H