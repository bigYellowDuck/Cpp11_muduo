#ifndef MUDUO_BASE_STRINGPIECE_H
#define MUDUO_BASE_STRINGPIECE_H

#include <string>
#include <string.h>
#include <assert.h>

namespace muduo {
    
class StringPiece {
public:
    StringPiece()
        : _ptr(nullptr),
          _length(0)
    {
    }

    StringPiece(const char* str)
        : _ptr(str),
          _length(strlen(_ptr))
    {
    }

    StringPiece(const unsigned char* str)
        : _ptr(reinterpret_cast<const char*>(str)),
          _length(strlen(_ptr))
    {
    }

    StringPiece(const std::string& str)
        : _ptr(str.data()),
          _length(static_cast<size_t>(str.length()))
    {
    }

    StringPiece(const char* offset, size_t len)
        : _ptr(offset),
          _length(len)
    {
    }

    const char* data() const { return _ptr; }

    size_t size() const { return _length; }

    bool empty() const { return _length == 0; }

    const char* begin() const { return _ptr; }

    const char* end() const { return _ptr + _length; }

    void clear()
    {
        _ptr = nullptr; 
        _length = 0; 
    }

    void set(const char* buffer, size_t len)
    {
        _ptr = buffer;
        _length = len;
    }

    void set(const char* str)
    {
        _ptr = str;
        _length = strlen(str);
    }

    void set(const void* buffer, size_t len)
    {
        _ptr = reinterpret_cast<const char*>(buffer);
        _length = len;
    }

    char operator[](size_t i) const 
    {   
        assert(i <  _length);
        return _ptr[i]; 
    }

    void remove_prefix(size_t n)
    {
        assert(n <= _length);
        _ptr += n;
        _length -= n;
    }

    void remove_suffix(size_t n)
    {
        assert(n <= _length);
        _length -= n;
    }

    bool operator==(const StringPiece& x) const
    {
        return ((_length == x._length) &&
                 (memcmp(_ptr, x._ptr, _length) == 0));
    }

    bool operator!=(const StringPiece& sp) const
    {
        return !(*this == sp);
    }

    std::string asString() const 
    { 
        return std::string(data(), size());
    } 

    void copyToStdString(std::string* target) const
    {
        assert(target != nullptr);
        target->assign(_ptr, _length);
    }

    bool startWith(const StringPiece& x) const
    {
        return ((_length >= x._length) && 
                (memcmp(_ptr, x._ptr, x._length) == 0));
    }

    int compare(const StringPiece& x) const 
    {
        int r = memcmp(_ptr, x._ptr, _length < x._length ? _length : x._length);
        if (r == 0)
        {
            if (_length < x._length) r = -1;
            else if (_length > x._length) r = +1;
        }
        return r;
    }
private:
    const char* _ptr;
    size_t _length;
};

}
#endif  // MUDUO_BASE_STRINGPIECE_H
