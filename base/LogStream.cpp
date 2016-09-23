#include "LogStream.h"
#include <algorithm>
#include <limits>

namespace muduo 
{

namespace detail 
{

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "sizeof digits is not 20");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof(digitsHex) == 17, "sizeof digitsHex is not 17");

// Effectient Integer to String Conversions, by Matthew Wilson
template <typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);
    

   *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

}   // end of namespace detail

void LogStream::staticCheck()
{
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<double>::digits10, "bit is not enough for double");
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long double>::digits10, "bit is not enough for long double");
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long>::digits10, "bit is not enough for double");
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long long>::digits10, "bit is not enough for double");
}

template <typename T>
void LogStream::formatInteger(T v)
{
    if (_buffer.avail() > kMaxNumericSize)
    {
        size_t len = detail::convert(_buffer.current(), v);
        _buffer.add(len);
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this <<static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
    uintptr_t i = reinterpret_cast<uintptr_t>(p);
    if (_buffer.avail() > kMaxNumericSize)
    {
        char* buf = _buffer.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = detail::convertHex(buf + 2, i);
        _buffer.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    // fixme
    return *this;
}

}   // end of namespace muduo

using namespace muduo;
using namespace muduo::detail;

