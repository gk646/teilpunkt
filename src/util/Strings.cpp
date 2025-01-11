#include <sodium/utils.h>
#include "util/Strings.h"

namespace tpunkt
{

    void ClearString(std::string& s)
    {
        sodium_memzero(s.data(), s.size());
        s.clear();
    }

} // namespace tpunkt