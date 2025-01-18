#ifndef TPUNKT_SECURE_LIST_H
#define TPUNKT_SECURE_LIST_H

#include <sodium/utils.h>
#include <type_traits>
#include "util/Memory.h"

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
                sodium_mprotect_readwrite(list.arr);
            }

            ~ListAccess()
            {
                sodium_mprotect_noaccess(list.arr);
            }


            T& operator[](const int idx)
            {
                if(idx >= list.size)
                {
                    LOG_FATAL("InternalCode : SecureList : Out of bounds access");
                }
                return list.arr[ idx ];
            }

            const T& operator[](const int idx) const
            {
                if(idx >= list.size)
                {
                    LOG_FATAL("InternalCode : SecureList : Out of bounds access");
                }
                return list.arr[ idx ];
            }

            // Adds a new element at the end
            void push_back(const T& value)
            {
                if(list.size == list.capacity)
                {
                    grow(list.capacity > 0 ? list.capacity * 2 : 10);
                }
                ::new(&list.arr[ list.size ]) T();
                list.arr[ list.size ] = value;
                ++list.size;
            }

            // Adds a new default element at the end and returns it
            T& emplace_back()
            {
                if(list.size == list.capacity)
                {
                    grow(list.capacity > 0 ? list.capacity * 2 : 10);
                }
                ::new(&list.arr[ list.size ]) T();
                return list.arr[ list.size++ ];
            }

            // Erases the first occurrence of the given value
            // DOES NOT KEEP ORDER
            bool erase(const T& value)
            {
                for(size_t i = 0; i < list.size; ++i)
                {
                    if(list.arr[ i ] == value)
                    {
                        if(i != list.size - 1) // Not at last place - copy last place to the matching one
                            memcpy(list.arr + i, list.arr + (list.size - 1), sizeof(T));
                        --list.size;
                        return true;
                    }
                }
                return false;
            }

            // Checks if the list contains the given value
            bool contains(const T& value) const
            {
                return findIndex(value) < list.size;
            }

            // Returns the position of the value or "size" if not found
            size_t findIndex(const T& value) const
            {
                for(size_t i = 0; i < list.size; ++i)
                {
                    if(list.arr[ i ] == value)
                    {
                        return i;
                    }
                }
                return list.size;
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
                return Iterator{list.arr + list.size};
            }
            Iterator begin()
            {
                return Iterator{list.arr};
            }
            Iterator end()
            {
                return Iterator{list.arr + list.size};
            }

          private:
            void grow(size_t newCapacity) const
            {
                if(newCapacity <= list.capacity)
                {
                    return;
                }
                T* newVal = TPUNKT_SECUREALLOC(newVal, sizeof(T) * newCapacity);

                for(size_t i = 0; i < newCapacity; ++i)
                {
                    ::new(&newVal[ i ]) T();
                }

                // Make old writeable
                sodium_mprotect_readwrite(list.arr);
                // Copy from old
                memcpy(newVal, list.arr, sizeof(T) * list.size);
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
            for(size_t i = 0; i < capacity; ++i)
            {
                ::new(&arr[ i ]) T();
            }
            sodium_mprotect_noaccess(arr);
        }

        SecureList(SecureList&& other) noexcept
        {
            arr = other.arr;
            size = other.size;
            capacity = other.capacity;
            other.arr = nullptr;
            other.capacity = 0;
            other.size = 0;
        }

        SecureList& operator=(SecureList&& other) noexcept
        {
            if(this == &other)
            {
                return *this;
            }
            arr = other.arr;
            size = other.size;
            capacity = other.capacity;
            other.arr = nullptr;
            other.capacity = 0;
            other.size = 0;
            return *this;
        }

        SecureList(const SecureList& other) = delete;
        SecureList& operator=(const SecureList& other) = delete;

        ~SecureList()
        {
            sodium_mprotect_readwrite(arr);
            TPUNKT_SECUREFREE(arr);
        }

      private:
        T* arr = nullptr;
        size_t size = 0;
        size_t capacity = 0;
        static_assert(std::is_trivially_destructible_v<T>, "Detors are not called");
        static_assert(std::is_trivially_copyable_v<T>, "Uses memcpy to copy values");
    };


} // namespace tpunkt

#endif // TPUNKT_SECURE_LIST_H