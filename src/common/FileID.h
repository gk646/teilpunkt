// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_TEILPUNKT_FILEID_H
#define TPUNKT_TEILPUNKT_FILEID_H

namespace tpunkt
{

struct FileID final
{
    FileID() = default;

    FileID(const uint32_t uid, const EndpointID endpointId, const bool isDirectory)
        : uid(uid), endpoint(endpointId), directory(isDirectory)
    {
    }

    static FileID FromUint(uint64_t uint)
    {
    }

    static uint64_t ToUint()
    {
        uint64_t uint{};
        std::memcpy(&uint, &uint, sizeof(uint64_t));
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
    uint32_t uid = UINT32_MAX;
    EndpointID endpoint{UINT16_MAX};
    bool directory = false;
    // 1 byte left
};

} // namespace tpunkt

#endif // TPUNKT_TEILPUNKT_FILEID_H
