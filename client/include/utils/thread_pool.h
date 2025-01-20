#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool(const ThreadPool &) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;

    static ThreadPool *getInstance(size_t numThreads = 1) {
        static ThreadPool instance(numThreads);
        return &instance;
    }

    template<class F>
    void enqueue(F &&f) { {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

private:
    explicit ThreadPool(size_t numThreads);

    ~ThreadPool();

    std::vector<std::thread> workers;
    std::queue<std::function<void()> > tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};


#endif //THREAD_POOL_H
