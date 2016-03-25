#ifndef ENGINE_TIME_HPP
#define ENGINE_TIME_HPP

#include "DataFile.hpp"

#include <string>

namespace engine
{

class Time : public DataFile::Saveable
{
public:
    Time();
    Time(int hours, int minutes, int seconds);
    Time(int year, int month, int day, int hours, int minutes, int seconds);

    bool operator == (const Time &time) const;
    bool operator != (const Time &time) const;

    static Time getLocal();
    static Time getUTC();

    void expose(DataFile::Node &node) override;

    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHours() const;
    int getMinutes() const;
    int getSeconds() const;
    float getHoursAsFloat() const;

    std::string toString() const;

private:
    static int dateToDays(int year, int month, int day);
    static void daysToDate(int days, int &year, int &month, int &day);

    void normalize();

    int m_year, m_month, m_day, m_hours, m_minutes, m_seconds;
};

} // namespace engine

#endif // ENGINE_TIME_HPP
