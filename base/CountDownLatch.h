#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>
#include "Noncopyable.h"

namespace muduo {

// 倒计时
class CountDownLatch : public Noncopyable
{
public:
    explicit CountDownLatch(int count);  // 倒数几次
    void wait();                         // 等待计数为零
    void countDown();                    // 计数减一
private:
    mutable std::mutex _mutex;
    std::condition_variable _condition;
    int _count;
};

}

#endif   // MUDUO_BASE_COUNTDOWNLATCH_H 
