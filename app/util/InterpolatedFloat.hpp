#ifndef APP_INTERPOLATED_FLOAT_HPP
#define APP_INTERPOLATED_FLOAT_HPP

#include "InterpolationType.hpp"

namespace app
{

class InterpolatedFloat
{
public:
    InterpolatedFloat(float initialValue, InterpolationType type, float stepOrDuration, float smoothingStartAtDistance = 1.f);

    void update();
    void setTargetValue(float targetValue);
    void setValueWithoutInterpolation(float value);
    void setNewStep(float step);
    float getCurrentValue() const;
    float getTargetValue() const;

private:
    InterpolationType m_type;
    float m_stepOrDuration;
    float m_smoothingStartAtDistance;
    float m_currentValue;
    float m_interpolatingFromValue;
    float m_interpolatingToValue;
    double m_msWhenStartedInterpolation;
};

} // namespace app

#endif // APP_INTERPOLATED_FLOAT_HPP
