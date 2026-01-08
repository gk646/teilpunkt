// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_TEILPUNKT_FILEID_H
#define TPUNKT_TEILPUNKT_FILEID_H

#include <cstdint>

namespace tpunkt
{

struct FileID final
{
    FileID() = default;

    FileID(const uint32_t uid, const bool isDirectory) : uid(uid), directory(isDirectory)
    {
    }

    [[nodiscard]] bool isDirectory() const
    {
        return directory;
    }

    [[nodiscard]] bool isFile() const
    {
        return !directory;
    }

    [[nodiscard]] uint32_t getUID() const
    {
        return uid;
    }

    bool operator==(const FileID&) const = default;

  private:
    uint32_t uid = UINT32_MAX;
    bool directory = false;
    // 3 byte left
};

} // namespace tpunkt

#endif // TPUNKT_TEILPUNKT_FILEID_H
