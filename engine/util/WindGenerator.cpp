#include "WindGenerator.hpp"

namespace engine
{

WindGenerator::WindGenerator(float strength, float regularity)
    : m_strength{strength},
      m_regularity{regularity}
{
}

float WindGenerator::getStrength() const
{
    return m_strength;
}

void WindGenerator::setStrength(float strength)
{
    m_strength = strength;
}

float WindGenerator::getRegularity() const
{
    return m_regularity;
}

void WindGenerator::setRegularity(float regularity)
{
    m_regularity = regularity;
}

FloatVec2 WindGenerator::getWind(const FloatVec3 &pos, double time)
{
    double seed{(time + pos.x * 7.0 * std::cos(time / 120000.0) + pos.z * 7.0 * std::sin(time / 120000.0)) / 1000.0};
    double dir{2.0 * Math::k_pi * noise(seed / m_regularity)};
    double amp{m_strength * std::sin(seed)};

    return {static_cast <float> (amp * std::cos(dir)),
            static_cast <float> (amp * std::sin(dir))};
}

float WindGenerator::randGenerator(int x) const
{
    x = (x << 13) ^ x;
    return static_cast <float> (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float WindGenerator::cosInterpolator(float a, float b, float x) const
{
    double ft{x * 3.1415927};
    double f{(1.0 - std::cos(ft)) * 0.5};

    return  a * (1.0 - f) + b * f;
}

float WindGenerator::windSmoother(int x) const
{
    return randGenerator(x) / 2 + randGenerator(x - 1) / 4 + randGenerator(x + 1) / 4;
}

float WindGenerator::noiseInterpolate(float x) const
{
    auto intX = static_cast <int> (x);
    float fracX{x - intX};

    float v1{windSmoother(intX)};
    float v2{windSmoother(intX + 1)};

    return cosInterpolator(v1, v2, fracX);
}

float WindGenerator::noise(float x) const
{
    float total{};
    float p{0.5f};

    int n{4};

    float frequency{1.f};
    float amplitude{1.f};

    for(int i = 0; i < n; ++i) {
      total += noiseInterpolate(x * frequency) * amplitude;

      frequency += frequency;
      amplitude *= p;
    }

    return total;
}

} // namespace engine
