#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include "Noncopyable.h"
#include <pthread.h>

namespace muduo {

template <typename T>
class Singleton : public Noncopyable { 
public:
    Singleton() = delete;

    static T& instance() 
    {
        pthread_once(&_ponce, &Singleton::init);
        return *_value;
    }

private:
    ~Singleton();         // 不进行析构
    
    static void init()
    {
        _value = new T();
    }

    static pthread_once_t _ponce;
    static T *_value;
};

template <typename T>
pthread_once_t Singleton<T>::_ponce = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::_value = nullptr;

}

#endif // MUDUO_BASE_SINGLETON_H
