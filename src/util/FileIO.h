// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_FILEIO_H
#define TPUNKT_FILEIO_H

#include <cstdint>
#include "util/Macros.h"

namespace tpunkt
{

struct File final
{
    explicit File(const char* path);
    ~File();

    // -1 on error
    [[nodiscard]] int64_t size() const;

    size_t write(const unsigned char* data, size_t size);
    size_t read(unsigned char* data, size_t size);

    static bool Exists(const char* path);

  private:
    int fd = -1;
    uint32_t nonce = 0;
    TPUNKT_MACROS_STRUCT(File);
};

struct FileStream final
{

    explicit FileStream(const char* path, const CipherKey& key, size_t chunkSize);
    ~FileStream();

    [[nodiscard]] size_t size() const;

    size_t write(const unsigned char* data, size_t size);
    size_t read(unsigned char* data, size_t size);

  private:
    CipherKey key;
    const char* path;
    size_t chunkSize;
    int fd = -1;
    TPUNKT_MACROS_STRUCT(FileStream);
};


} // namespace tpunkt

#endif // TPUNKT_FILEIO_H