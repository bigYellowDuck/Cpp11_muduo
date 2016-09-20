#ifndef MUDUO_BASE_NONCOPYABLE_H
#define MUDUO_BASE_NONCOPYABLE_H

namespace {

class Noncopyable {
public:
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable& operator=(const Noncopyable &) = delete;
protected:
    Noncopyable() {}
    ~Noncopyable() {}
};

}

#endif // MUDUO_BASE_NONCOPYABLE_H
