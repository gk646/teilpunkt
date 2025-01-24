#include "storage/Storage.h"

namespace tpunkt
{
    StorageStatus::StorageStatus(const StorageStatusType status) : status(status)
    {
    }

    bool StorageStatus::isOK() const
    {
        return status == StorageStatusType::OK;
    }

    const char* StorageStatus::getErrorMessage() const
    {
        switch(status)
        {
            case StorageStatusType::INVALID:
                return "Invalid status";
            case StorageStatusType::OK:
                return "OK";
            case StorageStatusType::ERR_NOT_INITIALIZED:
                return "Not initialized";
            case StorageStatusType::ERR_NOT_ENOUGH_SPACE:
                return "Not enough space";
            case StorageStatusType::ERR_ACTION_DENIED:
                return "Action denied";
        }
        return "Unkown";
    }
} // namespace tpunkt