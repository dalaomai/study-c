#include "autoptr.hpp"
#include <string>
#include <iostream>

template <typename I>
struct iter_traits
{
    typedef typename I::value_type value_type;
};

template <typename T>
struct iter_traits<T *>
{
    typedef T value_type;
};

template <typename T>
struct iter_traits<const T *>
{
    typedef T value_type;
};

template <class T>
struct iter
{
    typedef T value_type;
    T *ptr;
    iter(T *p = 0) : ptr(p) {}
    T &operator*() const { return *ptr; }
};

template <typename I>
typename iter_traits<I>::value_type a(I ite)
{
    return *ite;
}

int main()
{
    iter<int> t(new int(8));
    std::cout << a(t);
}