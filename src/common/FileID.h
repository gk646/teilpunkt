// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_TEILPUNKT_FILEID_H
#define TPUNKT_TEILPUNKT_FILEID_H

#include "fwd.h"

namespace tpunkt
{

struct FileID final
{
    static FileID FromNum(uint64_t num);
    static uint64_t ToNum(FileID fid);

    FileID() = default;
    FileID(EndpointID endpoint, bool isDirectory);

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

    [[nodiscard]] EndpointID getEndpoint() const
    {
        return static_cast<EndpointID>(endpoint);
    }

    bool operator==(const FileID&) const = default;

  private:
    uint32_t uid = UINT32_MAX;                 // unique descriptor
    EndpointID endpoint = EndpointID::INVALID; // which endpoint
    bool directory = false;                    // If directory or file
};

static_assert(sizeof(FileID) <= sizeof(uint64_t), "FileID too big!");

} // namespace tpunkt

#endif // TPUNKT_TEILPUNKT_FILEID_H
