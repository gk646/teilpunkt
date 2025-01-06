#ifndef TPUNKT_FILE_H
#define TPUNKT_FILE_H

#include "config.h"

enum class FileType : uint8_t
{
    INVALID = 0,
    PLAIN_FILE,
    SYMLINK,
};


// File index stored separately from the data
// Encrypted with the admin secret (except user-mode files)
struct FileDisplayInfo final
{
    char Name[TPUNKT_FILE_NAME_LEN]{}; // Display Name
    uint64_t size{};                   // encrypted size
};

// Backend File structure as decrypted from persistent storage
struct FileData final
{
    char name[TPUNKT_FILE_NAME_LEN]{}; // Real File Name
    unsigned char* content = nullptr;
    uint64_t fileSize{};
    FileType type{};
};





#endif //TPUNKT_FILE_H