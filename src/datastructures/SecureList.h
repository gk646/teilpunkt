#ifndef TPUNKT_SECURE_LIST_H
#define TPUNKT_SECURE_LIST_H

#include <type_traits>
#include <sodium/utils.h>
#include "crypto/CryptoManager.h"
#include "datastructures/Iterator.h"
#include "util/Logging.h"
#include "util/Macros.h"
#include "util/Memory.h"

namespace tpunkt
{
// Secure wrapper for a list of objects - always accessed as a whole list if at all
template <typename T>
struct SecureList final
{
    // Scoped accessor class
    struct ListReader final
    {
        explicit ListReader(SecureList& originalList) : list(originalList)
        {
            if(list.arr == nullptr || list.hasReader)
            {
                LOG_CRITICAL("Made reader from moved box or box already has reader");
            }
            list.hasReader = true;
            sodium_mprotect_readwrite(list.arr);
            GetCryptoManager().decrypt(list.arr, list.elements * sizeof(T));
        }

        ~ListReader()
        {
            list.hasReader = false;
            GetCryptoManager().encrypt(list.arr, list.elements * sizeof(T));
            sodium_mprotect_noaccess(list.arr);
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
            list.arr[ list.elements ] = value;
            ++list.elements;
        }

        // Adds a new element at the end
        void push_back(T&& value)
        {
            if(list.elements == list.capacity)
            {
                grow(list.capacity > 0U ? list.capacity * 2U : 10U);
            }
            ::new(&list.arr[ list.elements ]) T();
            list.arr[ list.elements ] = std::move(value);
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
                        list.arr[ i ] = std::move(list.arr[ list.elements - 1U ]);
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
                list.arr[ idx ] = std::move(list.arr[ list.elements - 1U ]);
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

        Iterator<const T> begin() const
        {
            return Iterator{list.arr};
        }
        Iterator<const T> end() const
        {
            return Iterator{list.arr + list.elements};
        }
        Iterator<T> begin()
        {
            return Iterator{list.arr};
        }
        Iterator<T> end()
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

            // Copy from old
            for(size_t i = 0U; i < list.elements; ++i)
            {
                newVal[ i ] = std::move(list.arr[ i ]);
            }
            // Delete old
            TPUNKT_SECUREFREE(list.arr);
            // Assign new to old - will be protected in dtor
            list.arr = newVal;
            list.capacity = newCapacity;
        }
        SecureList& list;
        TPUNKT_MACROS_STRUCT(ListReader);
    };

    ListReader get()
    {
        return ListReader{*this};
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

    SecureList(SecureList&& other) noexcept : arr(other.arr), elements(other.elements), capacity(other.capacity)
    {
        if(hasReader || other.hasReader)
        {
            LOG_CRITICAL("No move with active reader");
        }
        other.arr = nullptr;
        other.capacity = 0U;
        other.elements = 0U;
    }

    SecureList& operator=(SecureList&& other) noexcept
    {
        if(hasReader || other.hasReader)
        {
            LOG_CRITICAL("No move with active reader");
        }
        if(this != &other)
        {
            arr = other.arr;
            elements = other.elements;
            capacity = other.capacity;
            other.arr = nullptr;
            other.capacity = 0U;
            other.elements = 0U;
        }
        return *this;
    }

    ~SecureList()
    {
        if(hasReader)
        {
            LOG_CRITICAL("Cant destroy with active reader");
        }
        if(arr)
        {
            sodium_mprotect_readwrite(arr);
        }
        TPUNKT_SECUREFREE(arr);
    }

    SecureList(const SecureList& other) = delete;
    SecureList& operator=(const SecureList& other) = delete;

  private:
    T* arr = nullptr;
    size_t elements = 0U;
    size_t capacity = 0U;
    mutable bool hasReader = false; // Keep track if we have a reader - if yes disallow moving and dtor
    static_assert(std::is_trivially_destructible_v<T>, "Detors are not called");
    static_assert(std::is_default_constructible_v<T>, "No params supported");
};


} // namespace tpunkt

#endif // TPUNKT_SECURE_LIST_H