#include "Logging.h"
#include "Timestamp.h"
#include "TimeZone.h"

#include <stdlib.h>
#include <stdio.h>

#include <thread>

namespace muduo 
{

thread_local char   t_errnobuf[512];
thread_local char   t_time[32];
thread_local time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

Logger::LogLevel initLogLevel()
{
    if (::getenv("MUDUO_LOG_TRACE"))
        return Logger::LogLevel::TRACE;
    else if (::getenv("MUDUO_LOG_DEBUG"))
        return Logger::LogLevel::DEBUG;
    else 
        return Logger::LogLevel::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = 
{
    "TRACE ",
    "DEBUF ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL "
};

// helper class for known string length at compile time
class T
{
 public:
    T(const char* str, size_t len)
        : _str(str),
          _len(len)
    {
        assert(strlen(_str) == _len);
    }
    
    const char* _str;
    const unsigned _len;
};

inline LogStream& operator<<(LogStream& s, T v)
{
    s.append(v._str, v._len);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
    s.append(v._data, v._size);
    return s;
}

void defaufltOutput(const char* msg, size_t len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);    
}

Logger::outputFunc g_output = defaufltOutput;
Logger::flushFunc g_flush = defaultFlush;
TimeZone g_logTimeZone;


namespace detail 
{

uint64_t get_thread_id()
{
    static_assert(sizeof(std::thread::id)==sizeof(uint64_t), 
                  "this function only works if size of thread::id us equal to the size of uint64_t");
    auto id = std::this_thread::get_id();
    uint64_t* ptr = (uint64_t*)&id;
    return *ptr;
}

} // end of namespace detail

class Logger::Impl
{
 public:
    using LogLevel = Logger::LogLevel;
    Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
    void formatTime();
    void finish();

    Timestamp _time;
    LogStream _stream;
    LogLevel _level;
    int _line;
    SourceFile _basename;
};

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
    : _time(Timestamp::now()),
      _stream(),
      _level(level),
      _line(line),
      _basename(file)
{
    formatTime();
    _stream << detail::get_thread_id() << " " <<  T(LogLevelName[level], 6);
    
    if (savedErrno != 0)
    {
        _stream << strerror_tl(savedErrno) << " (error=" << savedErrno <<") ";
    }
}

void Logger::Impl::formatTime()
{
    int64_t microSecondsSinceEpoch = _time.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
        if (g_logTimeZone)
        {
            tm_time = g_logTimeZone.toLocalTime(seconds);
        }
        else
        {
            ::gmtime_r(&seconds, &tm_time);
        }

        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
    }

    if (g_logTimeZone)
    {
        Fmt us(".%06d ", microseconds);
        assert(us.length() == 8);
        _stream << T(t_time, 17) << T(us.data(), 8);
    }
    else 
    {
        Fmt us(".%06dZ ", microseconds);
        assert(us.length() == 9);
        _stream << T(t_time, 17) << T(us.data(), 9);
    }
}


void Logger::Impl::finish()
{
    _stream << " - " << _basename << " : " << _line << '\n';
}

Logger::Logger(SourceFile file, int line)
    : _pImpl(new Impl(INFO, 0, file, line))
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
    : _pImpl(new Impl(level, 0, file, line))
{
    _pImpl->_stream << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
    : _pImpl(new Impl(level, 0, file, line))
{ 
}

Logger::Logger(SourceFile file, int line, bool toAbort)
    : _pImpl(new Impl(toAbort?FATAL:ERROR, errno, file, line))
{
}

Logger::~Logger()
{
    _pImpl->finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (_pImpl->_level == FATAL)
    {
        g_flush();
        abort();
    }
}

LogStream& Logger::stream()
{
    return _pImpl->_stream;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutput(outputFunc out)
{
    g_output = out;
}

void Logger::setFlush(flushFunc flush)
{
    g_flush = flush;
}

void Logger::setTimeZone(const TimeZone& tz)
{
    g_logTimeZone = tz;
}

}   // end of namespace muduo

