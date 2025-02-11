// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_SECURE_BOX_H
#define TPUNKT_SECURE_BOX_H

#include <sodium/utils.h>
#include "util/Macros.h"
#include "util/Logging.h"
#include "util/Memory.h"
#include "crypto/CryptoManager.h"

namespace tpunkt
{
    // Save access wrapper for a single element - element is accessed individually
    template <typename T>
    struct SecureBox final
    {
        template <bool isConst>
        struct BoxReader final
        {
            using BoxType = std::conditional_t<isConst, const SecureBox, SecureBox>;

            explicit BoxReader(BoxType& originalBox) : box(originalBox)
            {
                if(box.val == nullptr || box.hasReader)
                {
                    LOG_CRITICAL("Made reader from moved box or box already has reader");
                }
                box.hasReader = true;
                sodium_mprotect_readwrite(box.val);
                GetCryptoManager().decrypt(box.val, sizeof(T));
            }

            ~BoxReader()
            {
                box.hasReader = false;
                GetCryptoManager().encrypt(box.val, sizeof(T));
                sodium_mprotect_noaccess(box.val);
            }

            T& get()
            {
                return *box.val;
                LOG_FATAL("No value present");
            }

            const T& get() const
            {
                return *box.val;
                LOG_FATAL("No value present");
            }

          private:
            BoxType& box;
            bool boxValid = true;
            TPUNKT_MACROS_STRUCT(BoxReader);
        };

        SecureBox()
        {
            val = TPUNKT_SECUREALLOC(val, sizeof(T));
            GetCryptoManager().encrypt(val, sizeof(T));
            sodium_mprotect_noaccess(val);
        }

        SecureBox(SecureBox&& other) noexcept : val(other.val)
        {
            if(hasReader || other.hasReader)
            {
                LOG_CRITICAL("No move with active reader");
            }
            other.val = nullptr;
        }

        SecureBox& operator=(SecureBox&& other) noexcept
        {
            if(hasReader || other.hasReader)
            {
                LOG_CRITICAL("No move with active reader");
            }
            if(this != &other) [[likely]]
            {
                if(val)
                {
                    sodium_mprotect_readwrite(val);
                    TPUNKT_SECUREFREE(val);
                }
                val = other.val;
                other.val = nullptr;
            }
            return *this;
        }

        ~SecureBox()
        {
            if(hasReader)
            {
                LOG_CRITICAL("Cant destroy with active reader");
            }
            if(val)
            {
                sodium_mprotect_readwrite(val);
                TPUNKT_SECUREFREE(val);
                val = nullptr;
            }
        }

        SecureBox(const SecureBox& other) = delete;
        SecureBox& operator=(const SecureBox& other) = delete;

        BoxReader<false> get()
        {
            return BoxReader<false>{*this};
        }

        BoxReader<true> get() const
        {
            return BoxReader<true>{*this};
        }

      private:
        T* val = nullptr;
        mutable bool hasReader = false; // Keep track if we have a reader - if yes disallow moving and dtor
    };


} // namespace tpunkt
#endif // TPUNKT_SECURE_BOX_H