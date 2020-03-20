#pragma once
#include <mutex>
#include <condition_variable>
namespace zzj
{
    class Semaphore {
    public:
        Semaphore(int count_ = 0)
            : count(count_) {}

        inline void Notify()
        {
            std::unique_lock<std::mutex> lock(mtx);
            count++;
            cv.notify_one();
        }

        inline void Wait()
        {
            std::unique_lock<std::mutex> lock(mtx);

            while (count == 0) {
                cv.wait(lock);
            }
            count--;
        }
        inline void SetCount(int count) {
            this->count = count;
        }
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
    };
}

