#ifndef MUDUO_BASE_LOGGING_H
#define MUDUO_BASE_LOGGING_H

#include "LogStream.h"
#include <memory>

namespace muduo 
{

class TimeZone;

class Logger
{
 public:
    enum LogLevel 
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class SourceFile
    {
     public:
        template <int N>
        inline SourceFile(const char (&arr)[N])
            : _data(arr),
              _size(N-1)
        {
            const char* slash = strrchr(_data, '/');   // 找到文件名
            if (slash)
            {
                _data = slash + 1;
                _size -= static_cast<size_t>(_data - arr);
            }
        }
        
        explicit SourceFile(const char* filename)
            : _data(filename)
        {
            const char* slash = strrchr(filename, '/');
            if (slash)
            {
                _data = slash + 1;
            }
            _size = strlen(_data);
        }

        const char* _data;
        size_t _size;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream();

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    using outputFunc = void (*)(const char* msg, size_t len);
    using flushFunc = void (*)();
    static void setOutput(outputFunc);
    static void setFlush(flushFunc);
    static void setTimeZone(const TimeZone& tz);
 private:
    class Impl;
    std::unique_ptr<Impl> _pImpl;
};

}


#endif // MUDUO_BASE_LOGGING_H

