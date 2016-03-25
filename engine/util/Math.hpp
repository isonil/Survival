#ifndef ENGINE_MATH_HPP
#define ENGINE_MATH_HPP

#include "Range.hpp"

#include <algorithm>
#include <cmath>

namespace engine
{

class Math
{
public:
    static bool fuzzyCompare(int a, int b);
    static bool fuzzyCompare(float a, float b, float eps = k_defaultEps);
    static bool fuzzyCompare(double a, double b, double eps = k_defaultEps);
    static int clamp(int val, int from, int to);
    static float clamp(float val, float from, float to);
    static double clamp(double val, double from, double to);
    static int clamp01(int val);
    static float clamp01(float val);
    static double clamp01(double val);
    static float lerp(float from, float to, float t);
    static float inverseLerp(float from, float to, float val);
    static int getUpperPOT(int a);
    static float getUpperPOT(float a);
    static double getUpperPOT(double a);
    template <typename T> static int approxToInt(T val);
    template <typename T> static int roundToInt(T val);
    static float degToRad(float deg);
    static float radToDeg(float rad);
    static float getLoopedDistance(float from, float to, const FloatRange &interval);
    static float getLoopedDistanceWithSign(float from, float to, const FloatRange &interval);
    static float loopedLerp(float from, float to, float t, const FloatRange &interval);
    template <typename Container> static auto lerpMany(const Container &container, float length, float t);

    static constexpr float k_pi{3.1415926f};
    static constexpr float k_defaultEps{0.0001f};
    static constexpr float k_sqrt2{1.414214f};
};

inline bool Math::fuzzyCompare(int a, int b)
{
    return a == b;
}

inline bool Math::fuzzyCompare(float a, float b, float eps)
{
    return std::fabs(a - b) < eps;
}

inline bool Math::fuzzyCompare(double a, double b, double eps)
{
    return std::fabs(a - b) < eps;
}

inline int Math::clamp(int val, int from, int to)
{
    return std::max(from, std::min(val, to));
}

inline float Math::clamp(float val, float from, float to)
{
    return std::max(from, std::min(val, to));
}

inline double Math::clamp(double val, double from, double to)
{
    return std::max(from, std::min(val, to));
}

inline int Math::clamp01(int val)
{
    return clamp(val, 0, 1);
}

inline float Math::clamp01(float val)
{
    return clamp(val, 0.f, 1.f);
}

inline double Math::clamp01(double val)
{
    return clamp(val, 0.0, 1.0);
}

inline float Math::lerp(float from, float to, float t)
{
    return t * (to - from) + from;
}

inline float Math::inverseLerp(float from, float to, float val)
{
    return (val - from) / (to - from);
}

inline int Math::getUpperPOT(int a)
{
    int POT{1};

    while(a > POT) {
        POT *= 2;
    }

    return POT;
}

inline float Math::getUpperPOT(float a)
{
    int val = static_cast <int> (a);

    if(a > 0.f && !fuzzyCompare(a, static_cast <float> (val)))
        ++val;

    return getUpperPOT(val);
}

inline double Math::getUpperPOT(double a)
{
    int val = static_cast <int> (a);

    if(a > 0.0 && !fuzzyCompare(a, static_cast <double> (val)))
        ++val;

    return getUpperPOT(val);
}

template <typename T> inline int Math::approxToInt(T val)
{
    int val1 = val;
    int val2{val1 - 1};
    int val3{val1 + 1};

    if(fuzzyCompare(val, static_cast <T> (val2)))
        return val2;

    if(fuzzyCompare(val, static_cast <T> (val3)))
        return val3;

    return val1;
}

template <typename T> inline int Math::roundToInt(T val)
{
    if(val < 0)
        return static_cast <int> (val - 0.5);
    else
        return static_cast <int> (val + 0.5);
}

inline float Math::degToRad(float deg)
{
    return deg * k_pi / 180.f;
}

inline float Math::radToDeg(float rad)
{
    return rad * 180.f / k_pi;
}

inline float Math::getLoopedDistance(float from, float to, const FloatRange &interval)
{
    if(interval.isEmpty())
        return 0.f;

    return std::fabs(getLoopedDistanceWithSign(from, to, interval));
}

inline float Math::getLoopedDistanceWithSign(float from, float to, const FloatRange &interval)
{
    if(interval.isEmpty())
        return 0.f;

    float intervalLen{interval.getLength()};

    from = std::fmod(from - interval.from, intervalLen);
    to = std::fmod(to - interval.from, intervalLen);

    if(from < 0.f)
        from += intervalLen;

    if(to < 0.f)
        to += intervalLen;

    from += interval.from;
    to += interval.from;

    if(from == to)
        return 0.f;
    else if(from < to) {
        // IF-------F--T-----------IT

        float leftDist{std::fabs(from - interval.from) + std::fabs(interval.to - to)};
        float rightDist{std::fabs(to - from)};

        if(leftDist < rightDist)
            return -leftDist;
        else
            return rightDist;
    }
    else {
        // IF-------T--F-----------IT

        float leftDist{std::fabs(from - to)};
        float rightDist{std::fabs(interval.to - from) + std::fabs(to - interval.from)};

        if(leftDist < rightDist)
            return -leftDist;
        else
            return rightDist;
    }
}

inline float Math::loopedLerp(float from, float to, float t, const FloatRange &interval)
{
    if(interval.isEmpty())
        return from;

    float loopingDistanceWithSign{getLoopedDistanceWithSign(from, to, interval)};

    float ans{from + loopingDistanceWithSign * t};
    float intervalLen{interval.getLength()};

    ans = std::fmod(ans - interval.from, intervalLen);

    if(ans < 0.f)
        ans += intervalLen;

    ans += interval.from;

    return ans;
}

template <typename Container> auto Math::lerpMany(const Container &container, float length, float t)
{
    E_DASSERT(!container.empty(), "Container is empty.");

    if(container.size() == 1)
        return container[0].second;

    for(size_t i = 0; i < container.size(); ++i) {
        if(t <= container[i].first) {
            if(i == 0) {
                float length0{container[0].first - t};
                float lengthBack{t + length - container.back().first};
                float partLength{length0 + lengthBack};

                return container.back().second * (length0 / partLength) + container[0].second * (lengthBack / partLength);
            }
            else {
                float lengthI{container[i].first - t};
                float lengthIMin1{t - container[i - 1].first};
                float partLength{lengthI + lengthIMin1};

                return container[i - 1].second * (lengthI / partLength) + container[i].second * (lengthIMin1 / partLength);
            }
        }
    }

    // if we ended up here, then we must be between container.back().first and 'length'

    float length0{length - t + container[0].first};
    float lengthBack{t - container.back().first};
    float partLength{length0 + lengthBack};

    return container.back().second * (length0 / partLength) + container[0].second * (lengthBack / partLength);
}

} // namespace engine

#endif // ENGINE_MATH_HPP
