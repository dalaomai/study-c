#include "utils.h"
#include <thread>
#include <iostream>
#include <unistd.h>

typedef std::queue<int> IntQueue;

void insert(IntQueue *a, std::condition_variable *cond)
{
    for (int i = 0; i < 1000; i++)
    {
        a->push(i);
        cond->notify_all();
        sleep(1);
    }
}

void push(int t, IntQueue *a, std::condition_variable *cond)
{
    int i;
    while (1)
    {
        while (!a->empty())
        {
            i = a->front();
            a->pop();
            std::cout << t << "  " << i << std::endl
                      << std::flush;
        }
        std::mutex temp_mutex;
        std::unique_lock<std::mutex> temp_lock(temp_mutex);
        cond->wait(temp_lock, [a]
                   { return !a->empty(); });
        temp_lock.unlock();
    }
}

void pop(IntQueue *a, std::condition_variable *cond)
{
    while (true)
    {
        std::cout << "aaa" << std::endl;
        std::mutex temp_mutex;
        std::unique_lock<std::mutex> temp_lock(temp_mutex);
        cond->wait(temp_lock, [a]
                   { return !a->empty(); });
        temp_lock.unlock();
    }
}

int main()
{
    IntQueue *a, *b;
    std::condition_variable cond;
    a = new IntQueue();

    std::thread t2(push, 1, a, &cond);
    // std::thread t3(pop, a, &cond);

    std::thread t1(insert, a, &cond);

    sleep(10);
    return 0;

    t1.join();
    t2.join();
    // t3.join();



    return 0;
}