#ifndef ENGINE_APP_TIME_HPP
#define ENGINE_APP_TIME_HPP

#include "Trace.hpp"
#include "DataFile.hpp"
#include "Time.hpp"

#include <QElapsedTimer>

namespace engine
{

class AppTime : public DataFile::Saveable, public Tracked <AppTime>
{
public:
    AppTime();

    void expose(DataFile::Node &node) override;

    void update();

    double getElapsedMs() const;
    double getDelta() const;
    double getDeltaAsSeconds() const;

private:
    static const double k_minDelta;
    static const double k_maxDelta;

    QElapsedTimer m_elapsedTimer;
    qint64 m_prevNsecs;
    double m_elapsedMs;
    double m_deltaMs;
};

} // namespace engine

#endif // ENGINE_APP_TIME_HPP
