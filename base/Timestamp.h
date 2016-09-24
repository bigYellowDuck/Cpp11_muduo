#ifndef MUDUO_BASE_TIMESTAMP_H
#define MUDUO_BASE_TIMESTAMP_H

#include <stdint.h>
#include <string>

namespace muduo
{

class Timestamp
{
 public:
    Timestamp()
        : _microSecondsSinceEpoch(0)
    {
    }

    explicit Timestamp(int64_t microSecondsSinceEpoch)
        : _microSecondsSinceEpoch(microSecondsSinceEpoch)
    {
    }

    // default copy/assignment/dtor are Okay

    void swap(Timestamp& rhs)
    {
        std::swap(_microSecondsSinceEpoch, rhs._microSecondsSinceEpoch);
    }

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;


    int64_t microSecondsSinceEpoch() const { return _microSecondsSinceEpoch; }
    time_t secondsSinceEpoch() const
    { return static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond); }

    // 静态方法，得到现在的时间
    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }

    static const int kMicroSecondsPerSecond;
 private:
    int64_t _microSecondsSinceEpoch;
};

const int Timestamp::kMicroSecondsPerSecond = 1000 * 1000;


inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator>(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
}

inline bool operator<=(Timestamp lhs, Timestamp rhs)
{
    return !(lhs > rhs);
}

inline bool operator>=(Timestamp lhs, Timestamp rhs)
{
    return !(lhs < rhs);
}

// 获取两个时间戳之间到差值，返回秒
inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond; 
}

// 在时间戳上加秒值
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}


}  // end of namespace muduo


#endif // MUDUO_BASE_TIMESTAMP_H

