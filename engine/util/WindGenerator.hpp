#ifndef ENGINE_WIND_GENERATOR_HPP
#define ENGINE_WIND_GENERATOR_HPP

#include "Vec2.hpp"
#include "Vec3.hpp"

namespace engine
{

class WindGenerator
{
public:
    WindGenerator(float strength, float regularity);

    float getStrength() const;
    void setStrength(float strength);

    float getRegularity() const;
    void setRegularity(float regularity);

    FloatVec2 getWind(const FloatVec3 &pos, double time);

private:
    float randGenerator(int x) const;
    float cosInterpolator(float a, float b, float x) const;
    float windSmoother(int x) const;
    float noiseInterpolate(float x) const;
    float noise(float x) const;

    float m_strength;
    float m_regularity;
};

} // namespace engine

#endif // ENGINE_WIND_GENERATOR_HPP
