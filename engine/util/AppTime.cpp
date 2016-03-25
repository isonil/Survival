#include "AppTime.hpp"

#include "Math.hpp"

namespace engine
{

AppTime::AppTime()
    : m_prevNsecs{0},
      m_elapsedMs{0.0},
      m_deltaMs{0.0}
{
    m_elapsedTimer.start();
}

void AppTime::expose(DataFile::Node &node)
{
    node.var(m_elapsedMs, "elapsedMs");
    node.var(m_deltaMs, "deltaMs");
}

void AppTime::update()
{
    TRACK;

    qint64 nowNsecs{m_elapsedTimer.nsecsElapsed()};

    m_deltaMs = (nowNsecs - m_prevNsecs) * 0.000001;
    m_deltaMs = Math::clamp(m_deltaMs, k_minDelta, k_maxDelta);

    m_elapsedMs += m_deltaMs;
    m_prevNsecs = nowNsecs;
}

double AppTime::getElapsedMs() const
{
    return m_elapsedMs;
}

double AppTime::getDelta() const
{
    return m_deltaMs;
}

double AppTime::getDeltaAsSeconds() const
{
    return getDelta() * 0.001;
}

const double AppTime::k_minDelta{0.1};
const double AppTime::k_maxDelta{50.0};

} // namespace engine
