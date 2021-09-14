#include <iostream>
template <typename T>
class ListItem
{
public:
    T value() const
    {
        return _value;
    }
    ListItem *next() const
    {
        return _next;
    }

private:
    T _value;
    ListItem *_next;
}

template <typename T>
class List
{
    void insert_font(T value);
    void insert_end(T value);
    void display(std::ostream &os = std::cout) const;

private:
    ListItem<T> *_end;
    ListItem<T> *_front;
    long _size;
}

template <class Item>
struct ListIter
{
    Item *ptr;
    ListIter(Item *p = 0) : ptr(p) {}

    Item &operator*() const { return *ptr; }
    Item &operator->() const { return ptr; }

    ListIt
}