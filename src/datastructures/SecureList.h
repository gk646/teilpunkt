#ifndef TPUNKT_SECURE_LIST_H
#define TPUNKT_SECURE_LIST_H

#include <sodium/utils.h>
#include <type_traits>
#include <cstring>
#include "util/Memory.h"
#include "util/Macros.h"
#include "util/Logging.h"
#include "crypto/CryptoManager.h"

namespace tpunkt
{
    // Secure wrapper for a list of objects - always accessed as a whole list if at all
    template <typename T>
    struct SecureList final
    {

        // Scoped accessor class
        struct ListAccess final
        {
            explicit ListAccess(SecureList& originalList) : list(originalList)
            {
                if(list.arr) [[likely]]
                {
                    sodium_mprotect_readwrite(list.arr);
                    GetCryptoManager().decrypt(list.arr, list.elements * sizeof(T));
                }
            }

            ~ListAccess()
            {
                if(list.arr) [[likely]]
                {
                    GetCryptoManager().encrypt(list.arr, list.elements * sizeof(T));
                    sodium_mprotect_noaccess(list.arr);
                }
            }

            [[nodiscard]] size_t size() const
            {
                return list.elements;
            }

            T& operator[](const size_t idx)
            {
                if(idx >= list.elements)
                {
                    LOG_FATAL("InternalCode : SecureList : Out of bounds access");
                }
                return list.arr[ idx ];
            }

            const T& operator[](const size_t idx) const
            {
                if(idx >= list.elements)
                {
                    LOG_FATAL("InternalCode : SecureList : Out of bounds access");
                }
                return list.arr[ idx ];
            }

            // Adds a new element at the end
            void push_back(const T& value)
            {
                if(list.elements == list.capacity)
                {
                    grow(list.capacity > 0U ? list.capacity * 2U : 10U);
                }
                ::new(&list.arr[ list.elements ]) T();
                memcpy(list.arr + list.elements, &value, sizeof(T));
                ++list.elements;
            }

            // Adds a new default element at the end and returns it
            T& emplace_back()
            {
                if(list.elements == list.capacity)
                {
                    grow(list.capacity > 0U ? list.capacity * 2U : 10U);
                }
                ::new(&list.arr[ list.elements ]) T();
                return list.arr[ list.elements++ ];
            }

            // Erases the first occurrence of the given value
            // DOES NOT KEEP ORDER
            bool erase(const T& value)
            {
                for(size_t i = 0U; i < list.elements; ++i)
                {
                    if(list.arr[ i ] == value)
                    {
                        // Not at last place - copy last place to the matching one
                        if(i != list.elements - 1U) [[likely]]
                        {
                            memcpy(list.arr + i, list.arr + (list.elements - 1U), sizeof(T));
                        }
                        --list.elements;
                        return true;
                    }
                }
                return false;
            }

            // Erases the element at the given index
            // DOES NOT KEEP ORDER
            bool eraseIndex(const size_t idx)
            {
                if(idx >= list.elements)
                {
                    LOG_CRITICAL("Out of bounds erase");
                    return false;
                }

                if(idx != list.elements - 1U) [[likely]]
                {
                    memcpy(list.arr + idx, list.arr + (list.elements - 1U), sizeof(T));
                }
                --list.elements;
                return true;
            }

            // Checks if the list contains the given value
            bool contains(const T& value) const
            {
                return findIndex(value) < list.elements;
            }

            // Returns the position of the value or "size" if not found
            size_t findIndex(const T& value) const
            {
                for(size_t i = 0U; i < list.elements; ++i)
                {
                    if(list.arr[ i ] == value)
                    {
                        return i;
                    }
                }
                return list.elements;
            }

            struct Iterator final
            {
                explicit Iterator(T* ptr) : ptr(ptr)
                {
                }

                T& operator*()
                {
                    return *ptr;
                }

                Iterator& operator++()
                {
                    ++ptr;
                    return *this;
                }

                Iterator operator++(int)
                {
                    Iterator temp = *this;
                    ++(*this);
                    return temp;
                }

                bool operator==(const Iterator& other) const
                {
                    return ptr == other.ptr;
                }

                bool operator!=(const Iterator& other) const
                {
                    return ptr != other.ptr;
                }

              private:
                T* ptr;
            };

            Iterator begin() const
            {
                return Iterator{list.arr};
            }
            Iterator end() const
            {
                return Iterator{list.arr + list.elements};
            }
            Iterator begin()
            {
                return Iterator{list.arr};
            }
            Iterator end()
            {
                return Iterator{list.arr + list.elements};
            }

          private:
            void grow(size_t newCapacity) const
            {
                if(newCapacity <= list.capacity)
                {
                    return;
                }
                T* newVal = TPUNKT_SECUREALLOC(newVal, sizeof(T) * newCapacity);

                for(size_t i = 0U; i < newCapacity; ++i)
                {
                    ::new(&newVal[ i ]) T();
                }

                // Make old writeable
                sodium_mprotect_readwrite(list.arr);
                // Copy from old
                memcpy(newVal, list.arr, sizeof(T) * list.elements);
                // Delete old
                TPUNKT_SECUREFREE(list.arr);
                // Assign new to old - will be protected in dtor
                list.arr = newVal;
                list.capacity = newCapacity;
            }
            SecureList& list;
            TPUNKT_MACROS_STRUCT(ListAccess);
        };

        ListAccess get()
        {
            return ListAccess{*this};
        }

        explicit SecureList(const size_t initialCapacity = 10) : capacity(initialCapacity)
        {
            arr = TPUNKT_SECUREALLOC(arr, sizeof(T) * initialCapacity);
            for(size_t i = 0U; i < capacity; ++i)
            {
                ::new(&arr[ i ]) T();
            }
            sodium_mprotect_noaccess(arr);
        }

        SecureList(SecureList&& other) noexcept
        {
            arr = other.arr;
            elements = other.elements;
            capacity = other.capacity;
            other.arr = nullptr;
            other.capacity = 0U;
            other.elements = 0U;
        }

        SecureList& operator=(SecureList&& other) noexcept
        {
            if(this == &other)
            {
                return *this;
            }
            arr = other.arr;
            elements = other.elements;
            capacity = other.capacity;
            other.arr = nullptr;
            other.capacity = 0U;
            other.elements = 0U;
            return *this;
        }

        SecureList(const SecureList& other) = delete;
        SecureList& operator=(const SecureList& other) = delete;

        ~SecureList()
        {
            if(arr)
            {
                sodium_mprotect_readwrite(arr);
            }
            TPUNKT_SECUREFREE(arr);
        }

      private:
        T* arr = nullptr;
        size_t elements = 0U;
        size_t capacity = 0U;
        static_assert(std::is_trivially_destructible_v<T>, "Detors are not called");
        static_assert(std::is_trivially_copyable_v<T>, "Uses memcpy to copy values");
        static_assert(std::is_default_constructible_v<T>, "No params supported");
    };


} // namespace tpunkt

#endif // TPUNKT_SECURE_LIST_H