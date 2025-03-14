#include <catch_amalgamated.hpp>
#include "crypto/Ciphers.h"
#include "datastructures/Buffer.h"
#include "TestCommons.h"

using namespace tpunkt;

TEST_CASE("BlockCipher: Valid full-block encryption and decryption")
{
    TEST_INIT();

    std::string data(33, 'A');
    const auto* udata = reinterpret_cast<const unsigned char*>(data.c_str());
    const auto dataLen = data.size();

    Buffer eBuffer;
    Buffer dBuffer;

    const CipherKey key{"This is my secret key"};
    BlockCipher cipher{key};

    size_t minOutLen = BlockCipher::GetEncryptMinLen(dataLen);
    eBuffer.ensure(minOutLen);
    REQUIRE(eBuffer.capacity() >= minOutLen);

    // Encrypt
    bool encryptOk = cipher.encrypt(udata, dataLen, eBuffer.data(), eBuffer.capacity());
    REQUIRE(encryptOk);
    // Ensure the ciphertext is different from the plaintext.
    REQUIRE(std::memcmp(udata, eBuffer.data(), dataLen) != 0);

    // Decrypt
    dBuffer.ensure(dataLen);
    bool decryptOk = cipher.decrypt(eBuffer.data(), minOutLen, dBuffer.data(), dBuffer.capacity());
    REQUIRE(decryptOk);
    REQUIRE(std::memcmp(udata, dBuffer.data(), dataLen) == 0);
}

TEST_CASE("BlockCipher: Fails on insufficient output buffer")
{
    TEST_INIT();

    std::string data(32, 'A');
    const auto* udata = reinterpret_cast<const unsigned char*>(data.c_str());
    const auto dataLen = data.size();

    const CipherKey key{"This is my secret key"};
    BlockCipher cipher{key};

    size_t minOutLen = BlockCipher::GetEncryptMinLen(dataLen);
    Buffer eBuffer;

    bool encryptOk = cipher.encrypt(udata, dataLen, eBuffer.data(), 10);
    REQUIRE_FALSE(encryptOk);
}

TEST_CASE("StreamCipher: Multi-chunk encryption and decryption")
{
    TEST_INIT();

    Buffer eBuffer;
    Buffer dBuffer;
    std::string data(300, 'A');
    const auto* udata = (const unsigned char*)data.c_str();
    const auto dataLen = data.size();
    const CipherKey key{"This is my other secret key"};

    eBuffer.ensure(StreamCipherEncrypt::GetMinBufSize(dataLen, 10));
    StreamCipherEncrypt encryptC{key, 30, eBuffer.data(), eBuffer.capacity()};
    {
        size_t pos = 0;
        for(int i = 0; i < 10; ++i)
        {
            const bool isLast = i == 9;
            const auto res = encryptC.encrypt(udata + pos, encryptC.getChunkSize(), isLast);
            REQUIRE(res);
            pos += encryptC.getChunkSize();
        }
    }

    StreamCipherDecrypt decryptC{key, 30, eBuffer.data(), encryptC.getWritten()};
    dBuffer.ensure(StreamCipherDecrypt::GetMinBufSize(encryptC.getWritten(), 10));
    {
        size_t outpos = 0;
        for(int i = 0; i < 10; ++i)
        {
            const auto res = decryptC.decrypt(dBuffer.data() + outpos, encryptC.getChunkSize());
            REQUIRE(res);
            outpos += encryptC.getChunkSize();
        }
    }
    REQUIRE(memcmp(dBuffer.data(), udata, dataLen) == 0);
}

TEST_CASE("StreamCipher: Single-call encryption and decryption")
{
    TEST_INIT();
    std::string data(50, 'D');
    const auto* udata = reinterpret_cast<const unsigned char*>(data.c_str());
    const auto dataLen = data.size();

    const CipherKey key{"This is my other secret key"};
    Buffer eBuffer{StreamCipherEncrypt::GetMinBufSize(50, 1)};
    StreamCipherEncrypt eCipher{key, 50, eBuffer.data(), eBuffer.capacity()};

    bool encryptOk = eCipher.encrypt(udata, 50, true);
    REQUIRE(encryptOk);

    Buffer dBuffer{StreamCipherDecrypt::GetMinBufSize(eCipher.getWritten(), 1)};

    StreamCipherDecrypt decipher{key, 50, eBuffer.data(), eCipher.getWritten()};
    bool decryptOk = decipher.decrypt(dBuffer.data(), dBuffer.capacity());
    REQUIRE(decryptOk);
    REQUIRE(std::memcmp(udata, dBuffer.data(), dataLen) == 0);
}

TEST_CASE("StreamCipher: Input sanitation and boundary check")
{
    TEST_INIT();
    unsigned char header[ 24 ];
    {
        StreamCipherEncrypt encrypt{{}, 10, header, 25};
        // not enough space
        REQUIRE_FALSE(encrypt.encrypt(header, 1, false));
    }
    {
        StreamCipherEncrypt encrypt{{}, 10, header, 50};
        // Input is not chunk size
        REQUIRE_FALSE(encrypt.encrypt(header, 1, false));
    }
    {
        StreamCipherDecrypt decrypt{{}, 10, header, 25};
        // Not enough space to read
        REQUIRE_FALSE(decrypt.decrypt(header, 1));
    }
    {
        StreamCipherDecrypt decrypt{{}, 10, header, 50};
        // Not enough space to write
        REQUIRE_FALSE(decrypt.decrypt(header, 9));
    }
}