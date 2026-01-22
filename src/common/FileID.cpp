// SPDX-License-Identifier: GPL-3.0-only
#include <cstring>
#include "common/FileID.h"
#include "datastructures/Spinlock.h"
#include "datastructures/FixedString.h"
#include "util/Strings.h"

namespace tpunkt
{

inline Spinlock ID_LOCK{};
inline uint32_t UID = 1;

FileID FileID::FromString(const FixedString<32>& str)
{
    FileID fid{};
    uint64_t num{};
    if(!StringToNumber(str.view(), num))
    {
        LOG_CRITICAL("Failed to create FileID from string");
    }
    memcpy(&fid, &num, sizeof(uint64_t));
    return fid;
}

FixedString<32> FileID::ToString(FileID fid)
{
    uint64_t num{};
    memcpy(&num, &fid, sizeof(uint64_t));
    return FixedString<32>{num};
}

FileID FileID::Root(EndpointID endpoint)
{
    FileID fid{};
    fid.endpoint = endpoint;
    fid.directory = true;
    return fid;
}

FileID::FileID(EndpointID endpoint, const bool isDirectory) : endpoint(endpoint), directory(isDirectory)
{
    SpinlockGuard lock{ID_LOCK};
    uid = UID++;
}

} // namespace tpunkt
