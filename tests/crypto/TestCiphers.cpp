

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

    size_t minOutLen = BlockCipher::getEncryptMinLen(dataLen);
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

    size_t minOutLen = BlockCipher::getEncryptMinLen(dataLen);
    Buffer eBuffer;
    // Ensure buffer is too small.
    eBuffer.ensure(minOutLen - 1);

    bool encryptOk = cipher.encrypt(udata, dataLen, eBuffer.data(), eBuffer.capacity());
    REQUIRE_FALSE(encryptOk);
}

TEST_CASE("StreamCipher: Multi-chunk encryption and decryption")
{
    TEST_INIT();

    Buffer eBuffer;
    Buffer dBuffer;
    std::string data(20, 'A');
    const auto* udata = (const unsigned char*)data.c_str();
    const auto dataLen = data.size();
    const CipherKey key{"This is my other secret key"};
    StreamCipher cipher{key};
    const size_t eSize = 2;
    {
        size_t currPos = 0;
        for(int i = 0; i < 10; ++i)
        {
            const bool isLast = i == 9;
            eBuffer.ensure(StreamCipher::getEncryptMinLen(currPos + eSize));
            const auto res =
                cipher.encrypt(udata + currPos, eSize, eBuffer.data() + currPos, eBuffer.capacity() - currPos, isLast);
            REQUIRE(res);
            currPos += eSize;
        }
    }

    {
        size_t currPos = 0;
        for(int i = 0; i < 10; ++i)
        {
            const bool isFirst = i == 0;
            const bool isLast = i == 9;
            dBuffer.ensure(currPos + eSize);
            REQUIRE(cipher.decrypt(eBuffer.data() + currPos, eSize, dBuffer.data() + currPos,
                                   dBuffer.capacity() - currPos, isFirst, isLast));
            currPos += eSize;
        }
    }

    REQUIRE(memcmp(dBuffer.data(), udata, dataLen) == 0);
}

TEST_CASE("StreamCipher: Single-call encryption and decryption")
{
    TEST_INIT();
    std::string data(20, 'D');
    const auto* udata = reinterpret_cast<const unsigned char*>(data.c_str());
    const auto dataLen = data.size();

    const CipherKey key{"This is my other secret key"};
    StreamCipher cipher{key};

    size_t minOutLen = StreamCipher::getEncryptMinLen(dataLen);
    Buffer eBuffer;
    Buffer dBuffer;
    eBuffer.ensure(minOutLen);

    bool encryptOk = cipher.encrypt(udata, dataLen, eBuffer.data(), eBuffer.capacity(), true);
    REQUIRE(encryptOk);

    // Use a new instance for decryption.
    StreamCipher decipher{key};
    dBuffer.ensure(dataLen);
    bool decryptOk = decipher.decrypt(eBuffer.data(), minOutLen, dBuffer.data(), dBuffer.capacity(), true, true);
    REQUIRE(decryptOk);
    REQUIRE(std::memcmp(udata, dBuffer.data(), dataLen) == 0);
}

TEST_CASE("StreamCipher: Fails on insufficient output buffer")
{
    std::string data(20, 'E');
    const auto* udata = reinterpret_cast<const unsigned char*>(data.c_str());
    const auto dataLen = data.size();

    const CipherKey key{"This is my other secret key"};
    StreamCipher cipher{key};

    size_t minOutLen = StreamCipher::getEncryptMinLen(dataLen);
    Buffer eBuffer;

    bool encryptOk = cipher.encrypt(udata, dataLen, eBuffer.data(), minOutLen - 31, true);
    REQUIRE_FALSE(encryptOk);
}