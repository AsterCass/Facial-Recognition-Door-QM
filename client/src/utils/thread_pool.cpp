#include "utils/thread_pool.h"

using namespace std;

ThreadPool::ThreadPool(size_t numThreads): stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                function<void()> task; {
                    unique_lock lock(this->queueMutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    task = move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}


ThreadPool::~ThreadPool() { {
        unique_lock lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (thread &worker: workers) {
        worker.join();
    }
}
