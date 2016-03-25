#ifndef ENGINE_VEC_2_HPP
#define ENGINE_VEC_2_HPP

#include "DataFile.hpp"
#include "Math.hpp"
#include "Range.hpp"

#include <cmath>

namespace engine
{

template <typename T> struct Vec2;

typedef Vec2 <int> IntVec2;
typedef Vec2 <float> FloatVec2;

template <typename T> struct Vec2 : public DataFile::Saveable
{
    Vec2();
    Vec2(T x, T y);

    Vec2 operator - () const;
    Vec2 operator + (const Vec2 &vec) const;
    Vec2 &operator += (const Vec2 &vec);
    Vec2 operator + (T val) const;
    Vec2 &operator += (T val);
    Vec2 operator - (const Vec2 &vec) const;
    Vec2 &operator -= (const Vec2 &vec);
    Vec2 operator - (T val) const;
    Vec2 &operator -= (T val);
    Vec2 operator * (const Vec2 &vec) const;
    Vec2 &operator *= (const Vec2 &vec);
    Vec2 operator * (T val) const;
    Vec2 &operator *= (T val);
    Vec2 operator / (const Vec2 &vec) const;
    Vec2 &operator /= (const Vec2 &vec);
    Vec2 operator / (T val) const;
    Vec2 &operator /= (T val);
    bool operator == (const Vec2 &vec) const;
    bool operator != (const Vec2 &vec) const;

    void expose(DataFile::Node &node) override;

    Vec2 &set(T x, T y);
    Vec2 &set(const Vec2 &vec);
    float getLength() const;
    float getLengthSq() const;
    T dotProduct(const Vec2 &vec) const;
    float getDistance(const Vec2 &vec) const;
    float getDistanceSq(const Vec2 &vec) const;
    float getLoopedDistance(const Vec2 &vec, const FloatRange &interval) const;
    float getLoopedDistanceSq(const Vec2 &vec, const FloatRange &interval) const;
    Vec2 &normalize();
    Vec2 normalized() const;
    Vec2 &setLength(T length);
    Vec2 &invert();
    Vec2 &squareComplement();
    Vec2 &POTComplement();
    Vec2 &moveCloserToBy(const Vec2 &other, float by);
    Vec2 &moveLoopedCloserToBy(const Vec2 &other, float by, const FloatRange &interval);
    Vec2 &move(T x, T y);
    Vec2 moved(T x, T y) const;
    Vec2 movedX(T x) const;
    Vec2 movedY(T y) const;
    Vec2 changedX(T x) const;
    Vec2 changedY(T y) const;
    bool isFuzzyZero() const;

    static Vec2 lerped(const Vec2 &first, const Vec2 &second, float t);
    static Vec2 loopedLerped(const Vec2 &first, const Vec2 &second, float t, const FloatRange &interval);

    T x, y;
};

template <typename T> inline Vec2 <T>::Vec2()
    :   x{}, y{}
{
}

template <typename T> inline Vec2 <T>::Vec2(T x, T y)
    :   x{x}, y{y}
{
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator - () const
{
    return {-x, -y};
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator + (const Vec2 &vec) const
{
    return {x + vec.x, y + vec.y};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator += (const Vec2 &vec)
{
    x += vec.x;
    y += vec.y;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator + (T val) const
{
    return {x + val, y + val};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator += (T val)
{
    x += val;
    y += val;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator - (const Vec2 &vec) const
{
    return {x - vec.x, y - vec.y};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator -= (const Vec2 &vec)
{
    x -= vec.x;
    y -= vec.y;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator - (T val) const
{
    return {x - val, y - val};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator -= (T val)
{
    x -= val;
    y -= val;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator * (const Vec2 &vec) const
{
    return {x * vec.x, y * vec.y};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator *= (const Vec2 &vec)
{
    x *= vec.x;
    y *= vec.y;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator * (T val) const
{
    return {x * val, y * val};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator *= (T val)
{
    x *= val;
    y *= val;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator / (const Vec2 &vec) const
{
    return {x / vec.x, y / vec.y};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator /= (const Vec2 &vec)
{
    x /= vec.x;
    y /= vec.y;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::operator / (T val) const
{
    return {x / val, y / val};
}

template <typename T> inline Vec2 <T> &Vec2 <T>::operator /= (T val)
{
    x /= val;
    y /= val;
    return *this;
}

template <typename T> inline bool Vec2 <T>::operator == (const Vec2 &vec) const
{
    return x == vec.x && y == vec.y;
}

template <typename T> inline bool Vec2 <T>::operator != (const Vec2 &vec) const
{
    return !(*this == vec);
}

template <typename T> inline void Vec2 <T>::expose(DataFile::Node &node)
{
    node.var(x, "x");
    node.var(y, "y");
}

template <typename T> inline Vec2 <T> &Vec2 <T>::set(T x, T y)
{
    this->x = x;
    this->y = y;
    return *this;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::set(const Vec2 &vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}

template <typename T> inline float Vec2 <T>::getLength() const
{
    return std::hypot(x, y);
}

template <typename T> inline float Vec2 <T>::getLengthSq() const
{
    return x * x + y * y;
}

template <typename T> inline T Vec2 <T>::dotProduct(const Vec2 &vec) const
{
    return x * vec.x + y * vec.y;
}

template <typename T> inline float Vec2 <T>::getDistance(const Vec2 &vec) const
{
    return std::hypot(x - vec.x, y - vec.y);
}

template <typename T> inline float Vec2 <T>::getDistanceSq(const Vec2 &vec) const
{
    float dx = x - vec.x;
    float dy = y - vec.y;
    return dx * dx + dy * dy;
}

template <typename T> inline float Vec2 <T>::getLoopedDistance(const Vec2 &vec, const FloatRange &interval) const
{
    if(interval.isEmpty())
        return 0.f;

    return std::sqrt(getLoopedDistanceSq(vec, interval));
}

template <typename T> inline float Vec2 <T>::getLoopedDistanceSq(const Vec2 &vec, const FloatRange &interval) const
{
    if(interval.isEmpty())
        return 0.f;

    FloatVec2 dist{Math::getLoopedDistanceWithSign(x, vec.x, interval),
                   Math::getLoopedDistanceWithSign(y, vec.y, interval)};

    return dist.getLengthSq();
}

template <typename T> inline Vec2 <T> &Vec2 <T>::normalize()
{
    double length = x * x + y * y;

    if(length == 0.0) {
        x = 1;
        y = 0;
        return *this;
    }

    length = 1.0 / sqrt(length);

    x *= length;
    y *= length;

    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::normalized() const
{
    Vec2 <T> vec{*this};

    vec.normalize();

    return vec;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::setLength(T length)
{
    normalize();
    return (*this *= length);
}

template <typename T> inline Vec2 <T> &Vec2 <T>::invert()
{
    x = -x;
    y = -y;
    return *this;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::squareComplement()
{
    auto m = std::max(x, y);
    x = m;
    y = m;
    return *this;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::POTComplement()
{
    x = Math::getUpperPOT(x);
    y = Math::getUpperPOT(y);
    return *this;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::moveCloserToBy(const Vec2 &other, float by)
{
    if(getDistanceSq(other) <= by * by)
        *this = other;
    else
        *this += (other - *this).normalized() * by;

    return *this;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::moveLoopedCloserToBy(const Vec2 &other, float by, const FloatRange &interval)
{
    if(interval.isEmpty()) {
        x = 0;
        y = 0;
        return *this;
    }

    Vec2 <T> factors{Math::getLoopedDistanceWithSign(x, other.x, interval),
                     Math::getLoopedDistanceWithSign(y, other.y, interval)};

    if(factors.getLengthSq() <= by * by)
        *this = other;
    else {
        factors.normalize();
        *this += factors * by;
    }

    float intervalLen{interval.getLength()};

    x = std::fmod(x - interval.from, intervalLen);
    y = std::fmod(y - interval.from, intervalLen);

    if(x < 0.f)
        x += intervalLen;

    if(y < 0.f)
        y += intervalLen;

    x += interval.from;
    y += interval.from;

    return *this;
}

template <typename T> inline Vec2 <T> &Vec2 <T>::move(T x, T y)
{
    this->x += x;
    this->y += y;
    return *this;
}

template <typename T> inline Vec2 <T> Vec2 <T>::moved(T x, T y) const
{
    return {this->x + x, this->y + y};
}

template <typename T> inline Vec2 <T> Vec2 <T>::movedX(T x) const
{
    return {this->x + x, this->y};
}

template <typename T> inline Vec2 <T> Vec2 <T>::movedY(T y) const
{
    return {this->x, this->y + y};
}

template <typename T> inline Vec2 <T> Vec2 <T>::changedX(T x) const
{
    return {x, this->y};
}

template <typename T> inline Vec2 <T> Vec2 <T>::changedY(T y) const
{
    return {this->x, y};
}

template <typename T> inline bool Vec2 <T>::isFuzzyZero() const
{
    return Math::fuzzyCompare(x, static_cast <T> (0)) &&
           Math::fuzzyCompare(y, static_cast <T> (0));
}

template <typename T> inline Vec2 <T> Vec2 <T>::lerped(const Vec2 &first, const Vec2 &second, float t)
{
    return {first.x + (second.x - first.x) * t,
            first.y + (second.y - first.y) * t};
}

template <typename T> inline Vec2 <T> Vec2 <T>::loopedLerped(const Vec2 &first, const Vec2 &second, float t, const FloatRange &interval)
{
    if(interval.isEmpty())
        return first;

    return {Math::loopedLerp(first.x, second.x, t, interval),
            Math::loopedLerp(first.y, second.y, t, interval)};
}

} // namespace engine

#endif // ENGINE_VEC_2_HPP
