#ifndef TPUNKT_SECURE_BOX_H
#define TPUNKT_SECURE_BOX_H

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
                if(box.val)
                {
                    sodium_mprotect_readwrite(box.val);
                    GetCryptoManager().decrypt(box.val, sizeof(T));
                }
            }

            ~BoxReader()
            {
                if(box.val)
                {
                    GetCryptoManager().encrypt(box.val, sizeof(T));
                    sodium_mprotect_noaccess(box.val);
                }
            }

            T& get()
            {
                return *box.val;
            }

            const T& get() const
            {
                return *box.val;
            }

          private:
            BoxType& box;
            TPUNKT_MACROS_STRUCT(BoxReader);
        };

        SecureBox()
        {
            val = TPUNKT_SECUREALLOC(val, sizeof(T));
        }

        SecureBox(SecureBox&& other) noexcept
        {
            val = other.val;
            other.val = nullptr;
        }

        SecureBox& operator=(SecureBox&& other) noexcept
        {
            if(this == &other)
            {
                return *this;
            }
            val = other.val;
            other.val = nullptr;
            return *this;
        }

        SecureBox(const SecureBox& other) = delete;
        SecureBox& operator=(const SecureBox& other) = delete;

        ~SecureBox()
        {
            TPUNKT_SECUREFREE(val);
            val = nullptr;
        }

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
    };


} // namespace tpunkt
#endif // TPUNKT_SECURE_BOX_H