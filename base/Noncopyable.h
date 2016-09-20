#ifndef MUDUO_BASE_NONCOPYABLE_H
#define MUDUO_BASE_NONCOPYABLE_H

namespace muduo {

class Noncopyable {
public:
    Noncopyable(const Noncopyable &) = delete;
    const Noncopyable& operator=(const Noncopyable &) = delete;
protected:
    Noncopyable() {}
    ~Noncopyable() {}
};

}

#endif // MUDUO_BASE_NONCOPYABLE_H
