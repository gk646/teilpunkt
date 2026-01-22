// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_TEILPUNKT_FILEID_H
#define TPUNKT_TEILPUNKT_FILEID_H

#include "fwd.h"

namespace tpunkt
{

struct FileID final
{
    static FileID FromString(const FixedString<32>& num);
    static FixedString<32> ToString(FileID fid);

    static FileID Root(EndpointID endpoint);
    FileID() = default;
    FileID(EndpointID endpoint, bool isDirectory);


    [[nodiscard]] bool isValid() const
    {
        return uid != UINT32_MAX;
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

    [[nodiscard]] EndpointID getEndpoint() const
    {
        return endpoint;
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
