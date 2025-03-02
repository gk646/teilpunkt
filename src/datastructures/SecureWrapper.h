// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_SECURE_WRAPPER_H
#define TPUNKT_SECURE_WRAPPER_H

#include <utility>
#include "util/Macros.h"
#include "crypto/CryptoContext.h"

namespace tpunkt
{
    // Save access wrapper for a single in stack memory element - element is accessed individually
    template <typename T>
    struct SecureWrapper final
    {
        template <bool isConst>
        struct WrapperReader final
        {
            using WrapperType = std::conditional_t<isConst, const SecureWrapper, SecureWrapper>;

            explicit WrapperReader(WrapperType& originalWrapper) : wrapper(originalWrapper)
            {
                GetCryptoContext().decrypt(static_cast<void*>(&wrapper.val), sizeof(T));
            }

            ~WrapperReader()
            {
                GetCryptoContext().encrypt(static_cast<void*>(&wrapper.val), sizeof(T));
            }

            T& get()
            {
                return wrapper.val;
            }

            const T& get() const
            {
                return wrapper.val;
            }

          private:
            WrapperType& wrapper;
            TPUNKT_MACROS_STRUCT(WrapperReader);
        };

        SecureWrapper() : val()
        {
            GetCryptoContext().encrypt(&val, sizeof(T));
        }

        // No destructor - does neither decrypt nor zero the contents - might be reused

        template <typename... Args>
        explicit SecureWrapper(Args&&... args) : val(std::forward<Args>(args)...)
        {
            GetCryptoContext().encrypt(&val, sizeof(T));
        }

        WrapperReader<false> get()
        {
            return WrapperReader<false>{*this};
        }

        WrapperReader<true> get() const
        {
            return WrapperReader<true>{*this};
        }

      private:
        T val;
        static_assert(std::is_trivially_destructible_v<T>, "No destructors called");
        TPUNKT_MACROS_STRUCT(SecureWrapper);
    };

} // namespace tpunkt

#endif // TPUNKT_SECURE_WRAPPER_H