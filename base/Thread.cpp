#include "Thread.h"
#include "CurrentThread.h"
#include "Logging.h"
#include "Timestamp.h"

#include <stdio.h>
#include <time.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <assert.h>

namespace muduo
{

namespace CurrentThread
{

thread_local int t_cachedTid = 0;  
thread_local char t_tidString[32];
thread_local int t_tidStringLength = 6;   // tid最大值为32678，长度设为6合适
thread_local const char* t_threadName = "unknown";
static_assert(std::is_same<int, pid_t>::value, "type pid_t is not same to int");

}  // end of namespace CurrentThread

namespace detail
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));    
}

void afterFork()
{
    CurrentThread::t_cachedTid = 0;
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
}

class ThreadNameInitializer
{
 public:
    ThreadNameInitializer()
    {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init; 

struct ThreadData
{
    using ThreadFunc = muduo::Thread::ThreadFunc;
    ThreadFunc _func;
    std::string _name;
    std::weak_ptr<pid_t> _wkTid;

    ThreadData(const ThreadFunc& func, const std::string& name, const std::shared_ptr<pid_t>& tid)
        : _func(func),
          _name(name),
          _wkTid(tid)
    {
    }

    void runInThread()
    {
        pid_t tid = CurrentThread::tid();

        std::shared_ptr<pid_t> ptid = _wkTid.lock();
        if (ptid)
        {
            *ptid = tid;
            ptid.reset();
        }

        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);

        _func();
        
        CurrentThread::t_threadName = "finished";
    }
};

void startThread(ThreadData* data)
{
    data->runInThread();
    delete data;
}

}  // end of namespace detail


void CurrentThread::cachedTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = { 0, 0 };
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec / Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

std::atomic<int> Thread::_numCreated(0);

Thread::Thread(const ThreadFunc& func, DtorAction action, const std::string& n)
    : _action(action),
      _started(false),
      _joined(false),
      _detached(false),
      _thread(),
      _tid(new pid_t(0)),
      _func(func),
      _name(n)
{
    setDefaultName();
}

Thread::Thread(ThreadFunc&& func, DtorAction action, const std::string& n)
    : _action(action),
      _started(false),
      _joined(false),
      _detached(false),
      _thread(),
      _tid(new pid_t(0)),
      _func(std::move(func)),
      _name(n)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (_started)
    {
        if (_thread.joinable())
        {
            if (_action == DtorAction::join)
                _thread.join();
            else
                _thread.detach();
        }
    }
}


void Thread::setDefaultName()
{
    ++_numCreated;
    int num = _numCreated.load();
    
    if (_name.empty())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        _name = buf;
    }
}

void Thread::start()
{
    assert(!_started);
    assert(!_joined);
    _started = true;

    detail::ThreadData* data = new detail::ThreadData(std::move(_func), _name, _tid);
    _thread = std::move(std::thread(detail::startThread, data));
}

void Thread::join()
{
    assert(_started);
    assert(!_detached);
    assert(!_joined);
    _joined = true;
    _thread.join();
}

void Thread::detach()
{
    assert(_started);
    assert(!_joined);
    assert(!_detached);
    _detached = true;
    _thread.detach();
}

}  // end of namespace clog

