#ifndef TPUNKT_COLLECTOR_H
#define TPUNKT_COLLECTOR_H

#include <cstddef>
#include <type_traits>
#include "util/Logging.h"

namespace tpunkt
{
    // Memory agnostic fixed size container with list interface
    template <typename T>
    struct Collector final
    {

        Collector(void* memory, const size_t len) : array(static_cast<T*>(memory)), capacity_(len / sizeof(T))
        {
        }

        [[nodiscard]] bool full() const
        {
            return size_ >= capacity_;
        }

        bool push_back(const T& value)
        {
            if(size_ >= capacity_)
            {
                LOG_CRITICAL("Out of bounds access");
            }
            array[ size_ ] = value;
            ++size_;
            return full();
        }

        [[nodiscard]] size_t size() const
        {
            return size_;
        }

        void setHasAll()
        {
            sufficient = true;
        }

        // Returns true if it could collect all information
        [[nodiscard]] bool hasAll() const
        {
            return sufficient;
        }

      private:
        T* array;
        size_t capacity_;
        size_t size_{};
        bool sufficient = false;
        static_assert(std::is_trivially_destructible_v<T>, "Detors are not called");
    };


} // namespace tpunkt

#endif // TPUNKT_COLLECTOR_H