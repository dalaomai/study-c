#include <string>
#include <iostream>

template <typename T1, typename T2>
class Pair
{
public:
    T1 key;
    T2 value;
    Pair(T1 key, T2 value);
    bool operator<(const Pair<T1, T2> &p) const;
};

template <typename T1, typename T2>
Pair<T1, T2>::Pair(T1 key, T2 value)
{
    Pair::key = key;
    Pair::value = value;
}

template <typename T1, typename T2>
bool Pair<T1, T2>::operator<(const Pair<T1, T2> &p) const
{
    return key < p.key;
}

typedef Pair<std::string, int> PairS;

#ifndef UTILS_H
#define UTILS_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SharedQueue
{
public:
    SharedQueue();
    ~SharedQueue();

    T &pop();

    void push_back(const T &item);
    void push_back(T &&item);
    void push(const T &item);

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template <typename T>
SharedQueue<T>::SharedQueue() {}

template <typename T>
SharedQueue<T>::~SharedQueue() {}

template <typename T>
T &SharedQueue<T>::pop()
{

    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.size() == 0)
    {
        cond_.wait(mlock);
    }
    T &r = queue_.front();
    queue_.pop_front();
    return r;
}

template <typename T>
void SharedQueue<T>::push_back(const T &item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread
}

template <typename T>
void SharedQueue<T>::push_back(T &&item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread
}

template <typename T>
void SharedQueue<T>::push(const T &item)
{
    push_back(item);
}

#endif