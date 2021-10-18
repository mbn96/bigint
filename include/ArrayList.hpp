#ifndef __ARRAYLIST_H__
#define __ARRAYLIST_H__

#include "MBNdef.hpp"
#include "BaseIterator.hpp"
#include <stdexcept>
#include <algorithm>

namespace MBN
{
    template <typename T>
    class ArrayList;

    template <typename T>
    class ArrListIterator : public BaseIterator<T>
    {
    private:
        const ArrayList<T> *list;
        size_t curr_index;

    public:
        ArrListIterator(const ArrayList<T> *list) : list(list), curr_index(0) {}

        bool hasNext() const
        {
            return list->size > curr_index;
        }

        T &getNext()
        {
            return (*list)[curr_index++];
        }
    };

    template <typename T>
    class ArrayList
    {
    private:
        size_t size = 0, arr_len = 0;
        T *arr = nullptr;
        void grow_arr(size_t new_len)
        {
            if (arr_len > new_len)
            {
                throw std::invalid_argument("new_len smaller than previous len.");
            }

            auto temp = new T[new_len];
            std::copy(arr, arr + size, temp);
            delete[] arr;
            arr = temp;
            arr_len = new_len;
        }

    public:
        ArrayList() : size(0), arr_len(4)
        {
            arr = new T[arr_len];
        }

        ArrayList(size_t init_capacity) : size(0), arr_len(init_capacity)
        {
            arr = new T[arr_len];
        }

        ArrayList(const ArrayList<T> &other) : size(other.size), arr_len(other.arr_len)
        {
            // std::cout << "Here in deep copy.\n";
            arr = new T[arr_len];
            std::copy(other.arr, other.arr + size, this->arr);
        }

        ArrayList(ArrayList<T> &&other)
        {
            // std::cout << "Here in shallow copy.\n";

            swap(*this, other);
        }

        ~ArrayList()
        {
            // std::cout << "Here in custom destructor ArrList.\n";

            delete[] arr;
        }

        template <typename A>
        friend void swap(ArrayList<A> &self, ArrayList<A> &other);

        template <typename A>
        friend std::ostream &operator<<(std::ostream &strm, const ArrayList<A> &list);

        T &operator[](size_t index) const
        {
            if (index >= size)
            {
                throw std::invalid_argument("Index out of bound.");
            }

            return arr[index];
        }

        ArrayList<T> &operator=(ArrayList<T> other)
        {
            // std::cout << "Here in =op.\n";

            swap(*this, other);
            return *this;
        }

        ArrayList<T> operator+(const ArrayList<T> &other) const
        {
            ArrayList<T> new_list(this->size + other.size);
            new_list.size = this->size + other.size;
            std::copy(arr, arr + size, new_list.arr);
            std::copy(other.arr, other.arr + other.size, new_list.arr + size);
            return new_list;
        }

        ArrayList<T> operator+(ArrayList<T> &&other) const
        {
            return *this + other;
        }

        size_t getSize() const
        {
            return size;
        }

        std::int64_t indexOf(T item) const
        {
            for (size_t i = 0; i < size; i++)
            {
                if (arr[i] == item)
                {
                    return i;
                }
            }
            return -1;
        }

        void append(T item)
        {
            if (size == arr_len)
            {
                grow_arr(arr_len * 2);
            }
            arr[size++] = item;
        }

        ArrayList<T> &operator<<(T item)
        {
            append(item);
            return *this;
        }

        void insert(T item, size_t index)
        {
            if (index >= size)
            {
                throw std::invalid_argument("Index out of bound.");
            }
            if (size == arr_len)
            {
                size++;
                arr_len *= 2;
                auto temp = new T[arr_len];
                std::copy(arr, arr + index, temp);
                temp[index] = item;
                std::copy(arr + index, arr + size, temp + index + 1);
                delete[] arr;
                arr = temp;
            }
            else
            {
                for (auto elem = arr + (size - 1); elem >= arr + index; elem--)
                {
                    elem[1] = elem[0];
                }
                arr[index] = item;
                size++;
            }
        }

        T removeAt(size_t index)
        {
            if (index >= size)
            {
                throw std::invalid_argument("Index out of bound.");
            }
            T r_out = arr[index];
            std::copy(arr + index + 1, arr + size, arr + index);
            size--;
            return r_out;
        }

        T popLast()
        {
            return removeAt(size - 1);
        }

        void clear()
        {
            delete[] arr;
            size = 0;
            arr_len = 4;
            arr = new T[arr_len];
        }

        template <typename A>
        friend class MBN::ArrListIterator;

        ArrListIterator<T> iterator() const
        {
            ArrListIterator<T> iter(this);
            return iter;
        }
    };

    template <typename T>
    void swap(ArrayList<T> &self, ArrayList<T> &other)
    {
        using std::swap;

        // std::cout << "Here in custom swap ArrList.\n";
        // std::cout << self << std::endl;
        // std::cout << other << std::endl;

        swap(self.size, other.size);
        swap(self.arr_len, other.arr_len);
        swap(self.arr, other.arr);
    }

    template <typename T>
    std::ostream &operator<<(std::ostream &strm, const ArrayList<T> &list)
    {
        if (list.size)
        {
            strm << '[';
            for (size_t i = 0; i < list.size - 1; i++)
            {
                strm << " " << (int)list.arr[i] << ',';
            }
            strm << " " << (int)list.arr[list.size - 1] << ']';
        }
        else
        {
            strm << "[]";
        }

        return strm;
    }

} // namespace MBN

#endif // __ARRAYLIST_H__