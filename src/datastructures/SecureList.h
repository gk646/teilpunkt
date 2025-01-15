#ifndef TPUNKT_SECURE_LIST_H
#define TPUNKT_SECURE_LIST_H

#include <sodium/utils.h>
#include <type_traits>
#include "util/Memory.h"


namespace tpunkt
{

    template <typename T>
    struct SecureList final
    {
        // Scoped accessor class
        struct ListAccess final
        {
            explicit ListAccess(SecureList& originalList) : list(originalList)
            {
                sodium_mprotect_readwrite(list.val);
            }

            ~ListAccess()
            {
                sodium_mprotect_noaccess(list.val);
            }

            T& operator[](const int idx)
            {
                return list.val[ idx ];
            }

            const T& operator[](const int idx) const
            {
                return list.val[ idx ];
            }

            // Adds a new element at the end
            void push_back(const T& value)
            {
                if(list.size == list.capacity)
                {
                    grow(list.capacity > 0 ? list.capacity * 2 : 10);
                }
                list.val[ list.size++ ] = value;
            }

            // Erases the first occurrence of the given value
            // DOES NOT KEEP ORDER
            bool erase(const T& value)
            {
                for(size_t i = 0; i < list.size; ++i)
                {
                    if(list.val[ i ] == value)
                    {
                        if(i != list.size - 1) // Not at last place - copy last place to the matching one
                            memcpy(list.val + i * sizeof(T), list.val + (list.size - 1) * sizeof(T), sizeof(T));
                        --list.size;
                        return true;
                    }
                }
                return false;
            }

            // Checks if the list contains the given value
            bool contains(const T& value) const
            {
                for(size_t i = 0; i < list.size; ++i)
                {
                    if(list.val[ i ] == value)
                    {
                        return true;
                    }
                }
                return false;
            }

          private:
            void grow(size_t newCapacity)
            {
                if(newCapacity <= list.capacity)
                {
                    return;
                }
                T* newVal = TPUNKT_SECUREALLOC(newVal, sizeof(T) * newCapacity);

                // Make old writeable
                sodium_mprotect_readwrite(list.val);
                // Copy from old
                memcpy(newVal, list.val, sizeof(T) * list.size);
                // Delete old
                TPUNKT_SECUREFREE(list.val);
                // Assign new to old - will be protected in dtor
                list.val = newVal;
                list.capacity = newCapacity;
            }

            SecureList& list;
            TPUNKT_MACROS_STRUCT(ListAccess);
        };

        explicit SecureList(const size_t initialCapacity = 10) : capacity(initialCapacity)
        {
            val = TPUNKT_SECUREALLOC(val, sizeof(T) * initialCapacity);
            sodium_mprotect_noaccess(val);
        }

        ~SecureList()
        {
            sodium_mprotect_readwrite(val);
            TPUNKT_SECUREFREE(val);
        }

        ListAccess get()
        {
            return ListAccess{*this};
        }

      private:
        T* val = nullptr;
        size_t size = 0;
        size_t capacity = 0;
        TPUNKT_MACROS_STRUCT(SecureList);
        static_assert(std::is_trivially_constructible_v<T>, "Ctors are not called");
        static_assert(std::is_trivially_destructible_v<T>, "Detors are not called");
    };


} // namespace tpunkt

#endif // TPUNKT_SECURE_LIST_H