#include "Time.hpp"

#include "Trace.hpp"

#include <cmath>
#include <ctime>
#include <sstream>

namespace engine
{

Time::Time()
    :   m_year{}, m_month{1}, m_day{1}, m_hours{}, m_minutes{}, m_seconds{}
{
}

Time::Time(int hours, int minutes, int seconds)
    :   m_year{}, m_month{1}, m_day{1}, m_hours{hours}, m_minutes{minutes}, m_seconds{seconds}
{
    normalize();
}

Time::Time(int year, int month, int day, int hours, int minutes, int seconds)
    :   m_year{year}, m_month{month}, m_day{day}, m_hours{hours}, m_minutes{minutes}, m_seconds{seconds}
{
    normalize();
}

bool Time::operator == (const Time &time) const
{
    return m_year == time.m_year && m_month == time.m_month && m_day == time.m_day &&
           m_hours == time.m_hours && m_minutes == time.m_minutes && m_seconds == time.m_seconds;
}

bool Time::operator != (const Time &time) const
{
    return !(*this == time);
}

Time Time::getLocal()
{
    TRACK;

    time_t currentTimeTemp{};
    time(&currentTimeTemp);
    tm *local{localtime(&currentTimeTemp)};

    if(!local)
        throw Exception{"Could not get time (function returned nullptr)."};

    return {local->tm_year + 1900,
            local->tm_mon + 1,
            local->tm_mday,
            local->tm_hour,
            local->tm_min,
            local->tm_sec};
}

Time Time::getUTC()
{
    TRACK;

    time_t UTCTimeTemp{};
    gmtime(&UTCTimeTemp);
    tm *UTC{localtime(&UTCTimeTemp)};

    if(!UTC)
        throw Exception{"Could not get time (function returned nullptr)."};

    return {UTC->tm_year + 1900,
            UTC->tm_mon + 1,
            UTC->tm_mday,
            UTC->tm_hour,
            UTC->tm_min,
            UTC->tm_sec};
}

void Time::expose(DataFile::Node &node)
{
    node.var(m_year, "year");
    node.var(m_month, "month");
    node.var(m_day, "day");
    node.var(m_hours, "hours");
    node.var(m_minutes, "minutes");
    node.var(m_seconds, "seconds");
}

int Time::getYear() const
{
    return m_year;
}

int Time::getMonth() const
{
    return m_month;
}

int Time::getDay() const
{
    return m_day;
}

int Time::getHours() const
{
    return m_hours;
}

int Time::getMinutes() const
{
    return m_minutes;
}

int Time::getSeconds() const
{
    return m_seconds;
}

float Time::getHoursAsFloat() const
{
    return m_hours + m_minutes / 60.f + m_seconds / 3600.f;
}

std::string Time::toString() const
{
    TRACK;

    std::ostringstream oss;

    if(m_day < 10)
        oss << '0';
    oss << m_day << '.';

    if(m_month < 10)
        oss << '0';
    oss << m_month << '.'
        << m_year << ' ';

    if(m_hours < 10)
        oss << '0';
    oss << m_hours << ':';

    if(m_minutes < 10)
        oss << '0';
    oss << m_minutes;

    return oss.str();
}

void Time::normalize()
{
    TRACK;

    while(m_seconds < 0) {
        m_seconds += 60;
        --m_minutes;
    }

    while(m_seconds >= 60) {
        m_seconds -= 60;
        ++m_minutes;
    }

    while(m_minutes < 0) {
        m_minutes += 60;
        --m_hours;
    }

    while(m_minutes >= 60) {
        m_minutes -= 60;
        ++m_hours;
    }

    int daysDiff{};

    while(m_hours < 0) {
        m_hours += 24;
        --daysDiff;
    }

    while(m_hours >= 24) {
        m_hours -= 24;
        ++daysDiff;
    }

    int days{dateToDays(m_year, m_month, m_day)};
    days += daysDiff;

    daysToDate(days, m_year, m_month, m_day);
}

int Time::dateToDays(int year, int month, int day)
{
    TRACK;

    // algorithm used: http://home.roadrunner.com/~hinnant/date_algorithms.html

    static_assert(std::numeric_limits <unsigned int>::digits >= 18,
                  "Not usable with 16 bit unsigned integers.");
    static_assert(std::numeric_limits <int>::digits >= 20,
                  "Not usable with 16 bit signed integers");

    int y{year};
    unsigned int m = month;
    unsigned int d = day;
    y -= m <= 2;
    int era{(y >= 0 ? y : y - 399) / 400};
    unsigned int yoe{static_cast <unsigned int> (y - era * 400)};
    unsigned int doy{(153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1};
    unsigned int doe{yoe * 365 + yoe / 4 - yoe / 100 + doy};
    return era * 146097 + static_cast <int> (doe) - 719468;
}

void Time::daysToDate(int days, int &year, int &month, int &day)
{
    TRACK;

    // algorithm used: http://home.roadrunner.com/~hinnant/date_algorithms.html

    static_assert(std::numeric_limits <unsigned int>::digits >= 18,
                  "Not usable with 16 bit unsigned integers.");
    static_assert(std::numeric_limits <int>::digits >= 20,
                  "Not usable with 16 bit signed integers");

    int z{days};

    z += 719468;
    int era{(z >= 0 ? z : z - 146096) / 146097};
    unsigned int doe{static_cast <unsigned int> (z - era * 146097)};
    unsigned int yoe{(doe - doe / 1460 + doe / 36524 - doe / 146096) / 365};
    int y{static_cast <int> (yoe) + era * 400};
    unsigned int doy{doe - (365 * yoe + yoe / 4 - yoe / 100)};
    unsigned int mp{(5 * doy + 2) / 153};
    unsigned int d{doy - (153 * mp + 2) / 5 + 1};
    unsigned int m{mp + (mp < 10 ? 3 : -9)};

    year = y + static_cast <int> (m <= 2);
    month = m;
    day = d;
}

} // namespace engine
