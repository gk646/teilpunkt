

#include <catch_amalgamated.hpp>
#include "crypto/Ciphers.h"
#include "datastructures/Buffer.h"
#include "TestCommons.h"

using namespace tpunkt;

TEST_CASE("Ciphers Test")
{
    TEST_INIT();

    Buffer eBuffer;
    Buffer dBuffer;
    std::string data(20, 'A');
    const auto* udata = (const unsigned char*)data.c_str();
    const auto dataLen = data.size();

    SECTION("Block Cipher")
    {
        const CipherKey key{"This is my secret key"};
        BlockCipher cipher{key};

        eBuffer.ensure(BlockCipher::getEncryptMinLen(dataLen));
        REQUIRE(eBuffer.capacity() >= BlockCipher::getEncryptMinLen(dataLen));

        REQUIRE(cipher.encrypt(udata, dataLen, eBuffer.data(), eBuffer.capacity()));
        REQUIRE(memcmp(udata, eBuffer.data(), dataLen) != 0);

        dBuffer.ensure(dataLen);
        REQUIRE(cipher.decrypt(eBuffer.data(), eBuffer.capacity(), dBuffer.data(), dBuffer.capacity()));
        REQUIRE(memcmp(udata, dBuffer.data(), dataLen) == 0);
    }

    SECTION("Stream Cipher")
    {
        const CipherKey key{"This is my other secret key"};
        StreamCipher cipher{key};
        const size_t eSize = 2;

        {
            size_t currPos = 0;
            for(int i = 0; i < 10; ++i)
            {
                const bool isLast = i == 9;
                eBuffer.ensure(StreamCipher::getEncryptMinLen(currPos + eSize));
                REQUIRE(cipher.encrypt(udata + currPos, eSize, eBuffer.data() + currPos, eBuffer.capacity() - currPos,
                                       isLast));
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
}