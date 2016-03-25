#include "InterpolatedLoopedFloatVec3.hpp"

#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

InterpolatedLoopedFloatVec3::InterpolatedLoopedFloatVec3(const engine::FloatVec3 &initialVec, InterpolationType type, float stepOrDuration, const engine::FloatRange &loopedRange, float smoothingStartAtDistance)
    : m_type{type},
      m_stepOrDuration{stepOrDuration},
      m_smoothingStartAtDistance{smoothingStartAtDistance},
      m_loopedRange{loopedRange},
      m_currentVec{initialVec},
      m_interpolatingFromVec{initialVec},
      m_interpolatingToVec{initialVec},
      m_msWhenStartedInterpolation{Global::getCore().getAppTime().getElapsedMs()}
{
    if(m_stepOrDuration < 0.f)
        throw engine::Exception{"Interpolation step nor duration can be negative."};

    if(m_loopedRange.isEmpty())
        throw engine::Exception{"Interpolation looped range can't be empty."};

    if(m_smoothingStartAtDistance < 0.f)
        throw engine::Exception{"Distance can't be negative."};
}

void InterpolatedLoopedFloatVec3::update()
{
    TRACK;

    const auto &appTime = Global::getCore().getAppTime();

    if(m_type == InterpolationType::FixedStepLinear ||
       m_type == InterpolationType::FixedStepSmooth) {
        // m_stepOrDuration is step here

        double delta{appTime.getDeltaAsSeconds()};
        double step{delta * m_stepOrDuration};

        if(m_type == InterpolationType::FixedStepSmooth) {
            float smoothFactor{1.f};

            float distance{m_currentVec.getLoopedDistance(m_interpolatingToVec, m_loopedRange)};

            if(distance < m_smoothingStartAtDistance)
                smoothFactor = distance / m_smoothingStartAtDistance;

            step *= smoothFactor;
        }

        m_currentVec.moveLoopedCloserToBy(m_interpolatingToVec, step, m_loopedRange);
    }
    else if(m_type == InterpolationType::FixedDuration) {
        // m_stepOrDuration is duration here

        double elapsedMs{appTime.getElapsedMs()};

        if(engine::Math::fuzzyCompare(m_stepOrDuration, 0.f))
            m_currentVec = m_interpolatingToVec;
        else {
            float t{engine::Math::inverseLerp(m_msWhenStartedInterpolation, m_msWhenStartedInterpolation + m_stepOrDuration, elapsedMs)};
            t = engine::Math::clamp01(t);

            m_currentVec = engine::FloatVec3::loopedLerped(m_interpolatingFromVec, m_interpolatingToVec, t, m_loopedRange);
        }
    }
    else
        throw engine::Exception{"Interpolation type not handled."};
}

void InterpolatedLoopedFloatVec3::setTargetVec(const engine::FloatVec3 &targetVec)
{
    const auto &appTime = Global::getCore().getAppTime();

    m_interpolatingFromVec = m_currentVec;
    m_interpolatingToVec = targetVec;
    m_msWhenStartedInterpolation = appTime.getElapsedMs();
}

void InterpolatedLoopedFloatVec3::setVecWithoutInterpolation(const engine::FloatVec3 &vec)
{
    m_currentVec = vec;
    m_interpolatingFromVec = vec;
    m_interpolatingToVec = vec;
}

void InterpolatedLoopedFloatVec3::setNewStep(float step)
{
    if(m_type == InterpolationType::FixedStepLinear ||
       m_type == InterpolationType::FixedStepSmooth)
        m_stepOrDuration = step;
}

const engine::FloatVec3 &InterpolatedLoopedFloatVec3::getCurrentVec() const
{
    return m_currentVec;
}

const engine::FloatVec3 &InterpolatedLoopedFloatVec3::getTargetVec() const
{
    return m_interpolatingToVec;
}

} // namespace app
