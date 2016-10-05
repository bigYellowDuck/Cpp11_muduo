#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include <stdint.h>

namespace muduo
{

namespace CurrentThread
{

extern thread_local int t_cachedTid;  
extern thread_local char t_tidString[32];
extern thread_local int t_tidStringLength;
extern thread_local const char* t_threadName;
void cachedTid();

inline int tid()
{
    if (__builtin_expect(t_cachedTid == 0, 0)) // 告诉编译器缓存到tid大部分时候不等于0
    {
        cachedTid();
    }
    return t_cachedTid;
}

inline const char* tidString()
{
    return t_tidString;
}

inline int tidStringLength()
{
    return t_tidStringLength;
}

inline const char* name()
{
    return t_threadName;
}

bool isMainThread();

void sleepUsec(int64_t usec);

}  // end of namespace CurrentThread

}  // end of namespace muduo

#endif // MUDUO_BASE_CURRENTTHREAD_H

