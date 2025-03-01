// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STREAM_CIPHER_H
#define TPUNKT_STREAM_CIPHER_H

#include <sodium/crypto_secretstream_xchacha20poly1305.h>
#include "fwd.h"

namespace tpunkt
{

struct StreamCipher final
{
    explicit StreamCipher(const CipherKey& key);

    bool encrypt(const unsigned char* input, size_t inLen, unsigned char* out, size_t outLen, bool isLast);
    bool decrypt(const unsigned char* input, size_t inLen, unsigned char* out, size_t outLen, bool isLast);

    [[nodiscard]] bool isReady() const;

  private:
    crypto_secretstream_xchacha20poly1305_state st{};
    unsigned char header[ crypto_secretstream_xchacha20poly1305_HEADERBYTES ];
    bool ready = false;
    bool headUsed = false;
};

} // namespace tpunkt

#endif // TPUNKT_STREAM_CIPHER_H