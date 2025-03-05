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

    static size_t getEncryptMinLen(size_t ilen);

    bool encrypt(const unsigned char* input, size_t ilen, unsigned char* out, size_t olen);
    bool decrypt(const unsigned char* input, size_t ilen, unsigned char* out, size_t olen);

  private:
    CipherKey key;
};

struct StreamCipher final
{
    explicit StreamCipher(const CipherKey& key);

    static size_t getEncryptMinLen(size_t ilen);

    bool encrypt(const unsigned char* input, size_t ilen, unsigned char* out, size_t olen, bool isLast);
    bool decrypt(const unsigned char* input, size_t ilen, unsigned char* out, size_t olen, bool isFirst, bool isLast);

  private:
    crypto_secretstream_xchacha20poly1305_state state{};
    CipherKey key;
    unsigned char header[ crypto_secretstream_xchacha20poly1305_HEADERBYTES ]{};
    bool ready = false;
    bool headUsed = false;
};

} // namespace tpunkt

#endif // TPUNKT_STREAM_CIPHER_H