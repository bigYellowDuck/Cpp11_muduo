#include  "CountDownLatch.h"

namespace muduo {

CountDownLatch::CountDownLatch(int count) 
    : _mutex(),
      _condition(),
      _count(count)
{

}


void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this]{ return this->_count <= 0; });

}


void CountDownLatch::countDown()
{
    std::lock_guard<std::mutex> lock(_mutex);
    --_count;
    if (_count == 0)
        _condition.notify_all();
}

}
