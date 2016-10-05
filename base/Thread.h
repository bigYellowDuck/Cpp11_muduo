#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <string>

namespace muduo
{

class Thread
{
 public:
    using ThreadFunc = std::function<void ()>;

    enum class DtorAction{ join, detach };

    explicit Thread(const ThreadFunc&, DtorAction, const std::string& name = std::string());

    explicit Thread(ThreadFunc&&, DtorAction, const std::string& name = std::string());

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(Thread&&) = default;
    Thread& operator=(Thread&&) = default;

    ~Thread();

    void start();
    
    void join();  

    void detach(); 

    bool started() const noexcept { return _started; }
    pid_t tid() const noexcept { return *_tid; }
    const std::string& name() const { return _name; }

    static int numCreated() { return _numCreated.load(); }
    
 private:
    void setDefaultName();

    DtorAction _action;
    bool _started;
    bool _joined;
    bool _detached;
    std::thread _thread;
    std::shared_ptr<pid_t> _tid;
    ThreadFunc _func;
    std::string _name;

    static std::atomic<int> _numCreated;
};

}  // end of namespace muduo


#endif // MUDUO_BASE_THREAD_H
