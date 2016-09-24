#ifndef MUDUO_BASE_TIMEZONE_H
#define MUDUO_BASE_TIMEZONE_H

#include <time.h>
#include <memory>

namespace muduo
{

class TimeZone
{
 public:
    explicit TimeZone(const char* zonefile);
    TimeZone(int eastOfUtc, const char* tzname);
    TimeZone() {}  // an valid timezone

    // default copy ctor/assignment/dtor are OKay
    
    explicit operator bool() const
    {
        return _data != nullptr;
    }

    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    time_t fromLocalTime(const struct tm&) const;

    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
    static time_t fromUtcTime(const struct tm&);
    static time_t fromUtcTime(int year, int month, int day, int hour, int minute, int sceonds);
    struct Data;
 private:
    std::shared_ptr<Data> _data;
};

}


#endif // MUDUO_BASE_TIMEZONE_H
