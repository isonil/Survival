#ifndef APP_INTERPOLATED_LOOPED_FLOAT_VEC_3_HPP
#define APP_INTERPOLATED_LOOPED_FLOAT_VEC_3_HPP

#include "engine/util/Vec3.hpp"
#include "engine/util/Range.hpp"
#include "InterpolationType.hpp"

namespace app
{

class InterpolatedLoopedFloatVec3
{
public:
    InterpolatedLoopedFloatVec3(const engine::FloatVec3 &initialVec, InterpolationType type, float stepOrDuration, const engine::FloatRange &loopedRange, float smoothingStartAtDistance = 1.f);

    void update();
    void setTargetVec(const engine::FloatVec3 &targetVec);
    void setVecWithoutInterpolation(const engine::FloatVec3 &vec);
    void setNewStep(float step);
    const engine::FloatVec3 &getCurrentVec() const;
    const engine::FloatVec3 &getTargetVec() const;

private:
    InterpolationType m_type;
    float m_stepOrDuration;
    float m_smoothingStartAtDistance;
    engine::FloatRange m_loopedRange;
    engine::FloatVec3 m_currentVec;
    engine::FloatVec3 m_interpolatingFromVec;
    engine::FloatVec3 m_interpolatingToVec;
    double m_msWhenStartedInterpolation;
};

} // namespace app

#endif // APP_INTERPOLATED_LOOPED_FLOAT_VEC_3_HPP
