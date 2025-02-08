#ifndef TPUNKT_ITERATOR_H
#define TPUNKT_ITERATOR_H


namespace tpunkt
{
    template <typename T>
    struct Iterator final
    {
        explicit Iterator(T* initPtr) : ptr(initPtr)
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

} // namespace tpunkt

#endif // TPUNKT_ITERATOR_H