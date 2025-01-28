#ifndef TPUNKT_STACKVECTOR_H
#define TPUNKT_STACKVECTOR_H

#include <cstddef>
#include "datastructures/Iterator.h"

namespace tpunkt
{

    template <typename T, size_t N = 16>
    struct SmallVector
    {
        using size_type = size_t;

        SmallVector() : size_(0), capacity_(N), data_(stack_data_)
        {
        }

        ~SmallVector()
        {
            if(data_ != stack_data_)
            {
                delete[] data_;
            }
        }

        void push_back(const T& value)
        {
            ensure_capacity(size_ + 1);
            data_[ size_++ ] = value;
        }

        // ensures capacity - initializes all new values
        void resize(const size_type new_size, const T& val)
        {
            if(new_size > size_)
            {
                ensure_capacity(new_size);
                for(size_type i = size_; i < new_size; ++i)
                {
                    data_[ i ] = val;
                }
                size_ = new_size;
            }
        }

        T& operator[](size_type index)
        {
            assert(index < size_);
            return data_[ index ];
        }

        const T& operator[](size_type index) const
        {
            assert(index < size_);
            return data_[ index ];
        }

        size_type size() const
        {
            return size_;
        }

        size_type capacity() const
        {
            return capacity_;
        }

        const T* data() const
        {
            return data_;
        }

        T* data()
        {
            return data_;
        }

        bool empty() const
        {
            return size_ == 0;
        }

        using iterator = Iterator<T>;
        using const_iterator = Iterator<const T>;

        iterator begin()
        {
            return iterator(data_);
        }
        const_iterator begin() const
        {
            return const_iterator(data_);
        }
        iterator end()
        {
            return iterator(data_ + size_);
        }
        const_iterator end() const
        {
            return const_iterator(data_ + size_);
        }

      private:
        void ensure_capacity(size_type new_capacity)
        {
            if(new_capacity <= capacity_)
            {
                return;
            }

            new_capacity = capacity_ * 2;
            T* new_data = new T[ new_capacity ];

            memcpy(new_data, data_, size_ * sizeof(T));

            if(data_ != stack_data_)
            {
                delete[] data_;
            }

            data_ = new_data;
            capacity_ = new_capacity;
        }

        size_type size_;
        size_type capacity_;
        T* data_;
    };

} // namespace tpunkt

#endif // TPUNKT_STACKVECTOR_H