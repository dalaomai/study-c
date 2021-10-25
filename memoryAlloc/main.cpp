#include <iostream>
#include <memory>
using namespace std;

class T
{

public:
    int i;
    short int y;
    T(int i = 2, short int y = 3) : i(i), y(y)
    {
        cout << "构造函数。"
             << " " << this << " " << i << endl;
    }

    ~T()
    {
        cout << "析构函数。"
             << " " << this << endl;
    }

    void *operator new(size_t sz)
    {
        T *t = (T *)malloc(sz);
        cout << "内存分配。" << sizeof(T) << " " << sz << " " << sizeof(size_t) << " " << t << endl;

        return t;
    }

    void *operator new[](size_t sz)
    {
        cout << "内存批量分配"
             << " " << sz << endl;
        return operator new(sz);
    }

    void operator delete(void *p)
    {

        free(p);
        cout << "内存释放。"
             << " " << p << endl;

        return;
    }

    void operator delete[](void *p)
    {
        cout << "内存批量释放"
             << " " << *(size_t *)p << endl;
        return operator delete(p);
    }
};

template <typename T>
inline void vector_delete(T *ptr, std::size_t size)
{
    if (ptr)
    {
        while (size)
        {
            ptr[--size].~T();
        }
        std::free(ptr);
    }
}

template <typename T, typename... Args>
inline T *vector_new(std::size_t size, Args... args)
{
    T *result = static_cast<T *>(malloc(sizeof(T) * size));
    if (result == NULL)
        return result;
    std::size_t i = 0;
    try
    {
        for (i = 0; i < size; ++i)
            ::new (result + i) T(args...);
    }
    catch (...)
    {
        std::cerr << "shit happen" << std::endl;
        vector_delete(result, i);
    }
    return result;
}

int main()
{
    size_t len = 5;
    std::allocator<T> ta;
    auto p = ta.allocate(len);
    for (int i = 0; i < len; i++)
    {
        ta.construct(p + i, 3, 4);
    }
    // uninitialized_fill_n(p, len, 2);

    for (int i = 0; i < len; i++)
    {
        ta.destroy(p + i);
    }
    ta.deallocate(p, len);

    // T *p = vector_new<T>(len, 3, 4);

    return 0;
}