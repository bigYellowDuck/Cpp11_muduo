#ifndef MUDUO_BASE_LOGSTREAM_H
#define MUDUO_BASE_LOGSTREAM_H

#include "StringPiece.h"
#include "Noncopyable.h"
#include <string.h>
#include <assert.h>
#include <string>

namespace muduo 
{

namespace detail 
{

const int kSmallBuffer = 4000;
const int kBigBuffer = 4000 * 1000;

template <int Size>
class FixeBuffer : public Noncopyable 
{
public:
    FixeBuffer()
        : _cur(_data)
    {
       setCookie(cookieStart); 
    }


    ~FixeBuffer()
    {
        setCookie(cookieEnd);
    }

    void append(const char* buf, size_t len)
    {
        if (avail() > len)
        {
            memcpy(_cur, buf, len);
            _cur += len;
        } 
    }

    const char* data() const { return _data; }
    size_t length() const { return _cur - _data; }
    
    // 用来给用户直接修改_data
    size_t avail() const { return end() - _cur; }
    char* current() { return _cur; }
    void add(size_t len) { _cur += len; }

    void reset() { _cur = _data; }
    void bzero() { ::bzero(_data, sizeof(_data)); }

    // 用来给GDB使用
    const char* debugString();
    void setCookie(void (*cookie)()) { _cookie_t = cookie; }

    // 用来单元测试
    std::string toString() const { return std::string(_data, length()); }
    StringPiece toStringPiece() const {return StringPiece(_data, length()); }
private:
    const char* end() const { return _data + sizeof(_data); }
    
    static void cookieStart();
    static void cookieEnd();

    void (*_cookie_t)();
    char _data[Size];
    char* _cur;
};

template <int Size>
const char* FixeBuffer<Size>::debugString()
{
    _cur = '\0';
    return _data;
}

template <int Size>
void FixeBuffer<Size>::cookieStart()
{
}

template <int Size>
void FixeBuffer<Size>::cookieEnd()
{
}

}   // end of namespace detail

class LogStream : public Noncopyable 
{
    using self = LogStream;
public:
    using Buffer = detail::FixeBuffer<detail::kSmallBuffer>;

    self& operator<<(bool v)
    {
        _buffer.append(v?"1":"0", 1);
        return *this;
    }
    
    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);

    self& operator<<(const void*);

    self& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    self& operator<<(double);

    self& operator<<(char v)
    {
        _buffer.append(&v, 1);
        return *this;
    }

    self& operator<<(const char* str)
    {
        if (str) 
        {
            _buffer.append(str, strlen(str));
        }
        else 
        {
            _buffer.append("(null)", 6);
        }
        return *this;
    }

    self& operator<<(const unsigned char* str)
    {
        *this << reinterpret_cast<const char*>(str);
        return *this;
    }

    self& operator<<(const std::string& str)
    {
        _buffer.append(str.c_str(), str.size());
        return *this;
    }

    self& operator<<(const StringPiece& v)
    {
        _buffer.append(v.data(), v.size());
        return *this;
    }
    
    self& operator<<(const Buffer& v)
    {
        *this << v.toStringPiece();
        return *this;
    }

    void append(const char* data, size_t len) { _buffer.append(data, len); }
    const Buffer& buffer() const { return _buffer; }
    void resetBuffer() { _buffer.reset(); }

private:
    void staticCheck();

    template <typename T>
    void formatInteger(T);

    Buffer _buffer;

    static const int kMaxNumericSize = 32;
};

class Fmt : public Noncopyable
{
 public:
    template <typename T>
    Fmt(const char* fmt, T val);

    const char* data() const { return _buf; }
    size_t length() const { return _length; }
 
 private:
    char _buf[32];
    size_t _length;
};

template <typename T>
Fmt::Fmt(const char* fmt, T val)
{
    static_assert(std::is_arithmetic<T>::value == true, "val is not arithmetic");
    _length = snprintf(_buf, sizeof(_buf), fmt, val);
    assert(_length < sizeof(_buf));
}

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

}   // end of namespace muduo

#define LOG_TRACE if(muduo::Logger::LogLevel() <= muduo::Logger::TRACE) \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if(muduo::Logger::LogLevel() <= muduo::Logger::DEBUG) \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::DEBUG, __func__).stream()
#define LOG_INFO if(muduo::Logger::LogLevel() <= muduo::Logger::INFO) \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::INFO, __func__).stream()
#define LOG_WARN muduo::Logger(__FILE__, __LINE__, muduo::Logger::LOG_WARN).stream()
#define LOG_ERROR muduo::Logger(__FILE__, __LINE__, muduo::Logger::ERROR).stream()
#define LOG_FATAL muduo::Logger(__FILE__, __LINE__, muduo::Logger::FATAL).stream()
#define LOG_SYSERR muduo::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL muduo::Logger(__FILE__, __LINE__, true).stream()


#endif // MUDUO_BASE_LOGSTREAM_H

























