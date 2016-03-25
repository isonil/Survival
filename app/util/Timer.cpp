#include "Timer.hpp"

#include "engine/util/Math.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

Timer::Timer()
    : m_startTime{},
      m_endTime{}
{
}

Timer::Timer(double duration)
    : m_startTime{},
      m_endTime{}
{
    set(duration);
}

void Timer::expose(engine::DataFile::Node &node)
{
    node.var(m_startTime, "startTime");
}

void Timer::set(double duration)
{
    m_startTime = Global::getCore().getAppTime().getElapsedMs();
    m_endTime = m_startTime + duration;
}

bool Timer::passed() const
{
    return Global::getCore().getAppTime().getElapsedMs() >= m_endTime;
}

float Timer::getProgress() const
{
    if(engine::Math::fuzzyCompare(m_startTime, m_endTime))
        return 1.f;

    double curTime{Global::getCore().getAppTime().getElapsedMs()};

    double ret{(curTime - m_startTime) / (m_endTime - m_startTime)};

    return engine::Math::clamp01(ret);
}

} // namespace app
