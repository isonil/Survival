#include "InterpolatedFloat.hpp"

#include "engine/util/Math.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

InterpolatedFloat::InterpolatedFloat(float initialValue, InterpolationType type, float stepOrDuration, float smoothingStartAtDistance)
    : m_type{type},
      m_stepOrDuration{stepOrDuration},
      m_smoothingStartAtDistance{smoothingStartAtDistance},
      m_currentValue{initialValue},
      m_interpolatingFromValue{initialValue},
      m_interpolatingToValue{initialValue},
      m_msWhenStartedInterpolation{Global::getCore().getAppTime().getElapsedMs()}
{
    if(m_stepOrDuration < 0.f)
        throw engine::Exception{"Interpolation step nor duration can be negative."};

    if(m_smoothingStartAtDistance < 0.f)
        throw engine::Exception{"Distance can't be negative."};
}

void InterpolatedFloat::update()
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

            float distance{std::fabs(m_currentValue - m_interpolatingToValue)};

            if(distance < m_smoothingStartAtDistance)
                smoothFactor = distance / m_smoothingStartAtDistance;

            step *= smoothFactor;
        }

        if(std::fabs(m_interpolatingToValue - m_currentValue) < step)
            m_currentValue = m_interpolatingToValue;
        else if(m_currentValue < m_interpolatingToValue)
            m_currentValue += step;
        else
            m_currentValue -= step;
    }
    else if(m_type == InterpolationType::FixedDuration) {
        // m_stepOrDuration is duration here

        double elapsedMs{appTime.getElapsedMs()};

        if(engine::Math::fuzzyCompare(m_stepOrDuration, 0.f))
            m_currentValue = m_interpolatingToValue;
        else {
            float t{engine::Math::inverseLerp(m_msWhenStartedInterpolation, m_msWhenStartedInterpolation + m_stepOrDuration, elapsedMs)};
            t = engine::Math::clamp01(t);

            m_currentValue = engine::Math::lerp(m_interpolatingFromValue, m_interpolatingToValue, t);
        }
    }
    else
        throw engine::Exception{"Interpolation type not handled."};
}

void InterpolatedFloat::setTargetValue(float targetValue)
{
    const auto &appTime = Global::getCore().getAppTime();

    m_interpolatingFromValue = m_currentValue;
    m_interpolatingToValue = targetValue;
    m_msWhenStartedInterpolation = appTime.getElapsedMs();
}

void InterpolatedFloat::setValueWithoutInterpolation(float value)
{
    m_currentValue = value;
    m_interpolatingFromValue = value;
    m_interpolatingToValue = value;
}

void InterpolatedFloat::setNewStep(float step)
{
    if(m_type == InterpolationType::FixedStepLinear ||
       m_type == InterpolationType::FixedStepSmooth)
        m_stepOrDuration = step;
}

float InterpolatedFloat::getCurrentValue() const
{
    return m_currentValue;
}

float InterpolatedFloat::getTargetValue() const
{
    return m_interpolatingToValue;
}

} // namespace app
