#ifndef MUDUO_BASE_DATE_H
#define MUDUO_BASE_DATE_H

#include <string>

struct tm;

namespace muduo
{

// 公历日期
class Date 
{
 public:
    struct YearMonthDay
    {
        int year;
        int month;
        int day;
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    Date()
        : _julianDayNumber(0)
    {}


    Date(int year, int month, int day);

    explicit Date(int julianDayNumber)
        : _julianDayNumber(julianDayNumber)
    {}

    explicit Date(const struct tm&);

    // default copy ctor/assignment/dtor are Okay
    void swap(Date& rhs)
    {
        std::swap(_julianDayNumber, rhs._julianDayNumber);
    }

    explicit operator bool() const 
    {
        return _julianDayNumber > 0;
    }

    // 转换成yyyy-mm-dd的格式
    std::string toIsoStirng() const;

    struct YearMonthDay yearMonthDay() const;

    int year() const 
    {
        return yearMonthDay().year;
    }

    int month() const
    {
        return yearMonthDay().month;
    }

    int day() const
    {
        return yearMonthDay().day;
    }

    int weekDay() const
    {
        return (_julianDayNumber + 1) % kDaysPerWeek;
    }

    int julianDayNumber() const { return _julianDayNumber; }
 private:
    int _julianDayNumber;
};


inline bool operator<(Date x, Date y)
{
    return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y)
{
    return x.julianDayNumber() == y.julianDayNumber();
}


}  // end of namespace muduo

#endif // MUDUO_BASE_DATE_H

