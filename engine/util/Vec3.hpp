#ifndef ENGINE_VEC_3_HPP
#define ENGINE_VEC_3_HPP

#include "DataFile.hpp"
#include "Math.hpp"
#include "Range.hpp"

#include <cmath>

namespace engine
{

template <typename T> struct Vec3;

typedef Vec3 <int> IntVec3;
typedef Vec3 <float> FloatVec3;

template <typename T> struct Vec3 : public DataFile::Saveable
{
    Vec3();
    Vec3(T x, T y, T z);

    Vec3 operator - () const;
    Vec3 operator + (const Vec3 &vec) const;
    Vec3 &operator += (const Vec3 &vec);
    Vec3 operator + (T val) const;
    Vec3 &operator += (T val);
    Vec3 operator - (const Vec3 &vec) const;
    Vec3 &operator -= (const Vec3 &vec);
    Vec3 operator - (T val) const;
    Vec3 &operator -= (T val);
    Vec3 operator * (const Vec3 &vec) const;
    Vec3 &operator *= (const Vec3 &vec);
    Vec3 operator * (T val) const;
    Vec3 &operator *= (T val);
    Vec3 operator / (const Vec3 &vec) const;
    Vec3 &operator /= (const Vec3 &vec);
    Vec3 operator / (T val) const;
    Vec3 &operator /= (T val);
    bool operator == (const Vec3 &vec) const;
    bool operator != (const Vec3 &vec) const;

    void expose(DataFile::Node &node) override;

    Vec3 &set(T x, T y, T z);
    Vec3 &set(const Vec3 &vec);
    float getLength() const;
    float getLengthSq() const;
    T dotProduct(const Vec3 &vec) const;
    float getDistance(const Vec3 &vec) const;
    float getDistanceSq(const Vec3 &vec) const;
    float getLoopedDistance(const Vec3 &vec, const FloatRange &interval) const;
    float getLoopedDistanceSq(const Vec3 &vec, const FloatRange &interval) const;
    Vec3 crossProduct(const Vec3 &vec) const;
    Vec3 &normalize();
    Vec3 normalized() const;
    Vec3 &setLength(T length);
    Vec3 &invert();
    Vec3 &cubeComplement();
    Vec3 &POTComplement();
    Vec3 &moveCloserToBy(const Vec3 &other, float by);
    Vec3 &moveLoopedCloserToBy(const Vec3 &other, float by, const FloatRange &interval);
    Vec3 &move(T x, T y, T z);
    Vec3 moved(T x, T y, T z) const;
    Vec3 movedX(T x) const;
    Vec3 movedY(T y) const;
    Vec3 movedZ(T z) const;
    Vec3 changedX(T x) const;
    Vec3 changedY(T y) const;
    Vec3 changedZ(T z) const;
    bool isFuzzyZero() const;

    static Vec3 lerped(const Vec3 &first, const Vec3 &second, float t);
    static Vec3 loopedLerped(const Vec3 &first, const Vec3 &second, float t, const FloatRange &interval);

    T x, y, z;
};

template <typename T> inline Vec3 <T>::Vec3()
    :   x{}, y{}, z{}
{
}

template <typename T> inline Vec3 <T>::Vec3(T x, T y, T z)
    :   x{x}, y{y}, z{z}
{
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator - () const
{
    return {-x, -y, -z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator + (const Vec3 &vec) const
{
    return {x + vec.x, y + vec.y, z + vec.z};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator += (const Vec3 &vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator + (T val) const
{
    return {x + val, y + val, z + val};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator += (T val)
{
    x += val;
    y += val;
    z += val;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator - (const Vec3 &vec) const
{
    return {x - vec.x, y - vec.y, z - vec.z};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator -= (const Vec3 &vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator - (T val) const
{
    return {x - val, y - val, z - val};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator -= (T val)
{
    x -= val;
    y -= val;
    z -= val;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator * (const Vec3 &vec) const
{
    return {x * vec.x, y * vec.y, z * vec.z};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator *= (const Vec3 &vec)
{
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator * (T val) const
{
    return {x * val, y * val, z * val};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator *= (T val)
{
    x *= val;
    y *= val;
    z *= val;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator / (const Vec3 &vec) const
{
    return {x / vec.x, y / vec.y, z / vec.z};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator /= (const Vec3 &vec)
{
    x /= vec.x;
    y /= vec.y;
    z /= vec.z;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::operator / (T val) const
{
    return {x / val, y / val, z / val};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::operator /= (T val)
{
    x /= val;
    y /= val;
    z /= val;
    return *this;
}

template <typename T> inline bool Vec3 <T>::operator == (const Vec3 &vec) const
{
    return x == vec.x && y == vec.y && z == vec.z;
}

template <typename T> inline bool Vec3 <T>::operator != (const Vec3 &vec) const
{
    return !(*this == vec);
}

template <typename T> inline void Vec3 <T>::expose(DataFile::Node &node)
{
    node.var(x, "x");
    node.var(y, "y");
    node.var(z, "z");
}

template <typename T> inline Vec3 <T> &Vec3 <T>::set(T x, T y, T z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::set(const Vec3 &vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

template <typename T> inline float Vec3 <T>::getLength() const
{
    return std::sqrt(getLengthSq());
}

template <typename T> inline float Vec3 <T>::getLengthSq() const
{
    return x * x + y * y + z * z;
}

template <typename T> inline T Vec3 <T>::dotProduct(const Vec3 &vec) const
{
    return x * vec.x + y * vec.y + z * vec.z;
}

template <typename T> inline float Vec3 <T>::getDistance(const Vec3 &vec) const
{
    return std::sqrt(getDistanceSq(vec));
}

template <typename T> inline float Vec3 <T>::getDistanceSq(const Vec3 &vec) const
{
    float dx = x - vec.x;
    float dy = y - vec.y;
    float dz = z - vec.z;
    return dx * dx + dy * dy + dz * dz;
}

template <typename T> inline float Vec3 <T>::getLoopedDistance(const Vec3 &vec, const FloatRange &interval) const
{
    if(interval.isEmpty())
        return 0.f;

    return std::sqrt(getLoopedDistanceSq(vec, interval));
}

template <typename T> inline float Vec3 <T>::getLoopedDistanceSq(const Vec3 &vec, const FloatRange &interval) const
{
    if(interval.isEmpty())
        return 0.f;

    FloatVec3 dist{Math::getLoopedDistanceWithSign(x, vec.x, interval),
                   Math::getLoopedDistanceWithSign(y, vec.y, interval),
                   Math::getLoopedDistanceWithSign(z, vec.z, interval)};

    return dist.getLengthSq();
}

template <typename T> inline Vec3 <T> Vec3 <T>::crossProduct(const Vec3 &vec) const
{
    return {y * vec.z - z * vec.y,
            z * vec.x - x * vec.z,
            x * vec.y - y * vec.x};
}

template <typename T> inline Vec3 <T> &Vec3 <T>::normalize()
{
    double length = x * x + y * y + z * z;

    if(length == 0.0) {
        x = 1;
        y = 0;
        z = 0;
        return *this;
    }

    length = 1.0 / sqrt(length);

    x *= length;
    y *= length;
    z *= length;

    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::normalized() const
{
    Vec3 <T> vec{*this};

    vec.normalize();

    return vec;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::setLength(T length)
{
    normalize();
    return (*this *= length);
}

template <typename T> inline Vec3 <T> &Vec3 <T>::invert()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::cubeComplement()
{
    auto m = std::max({x, y, z});
    x = m;
    y = m;
    z = m;
    return *this;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::POTComplement()
{
    x = Math::getUpperPOT(x);
    y = Math::getUpperPOT(y);
    z = Math::getUpperPOT(z);
    return *this;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::moveCloserToBy(const Vec3 &other, float by)
{
    if(getDistanceSq(other) <= by * by)
        *this = other;
    else
        *this += (other - *this).normalized() * by;

    return *this;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::moveLoopedCloserToBy(const Vec3 &other, float by, const FloatRange &interval)
{
    if(interval.isEmpty()) {
        x = 0;
        y = 0;
        z = 0;
        return *this;
    }

    Vec3 <T> factors{Math::getLoopedDistanceWithSign(x, other.x, interval),
                     Math::getLoopedDistanceWithSign(y, other.y, interval),
                     Math::getLoopedDistanceWithSign(z, other.z, interval)};

    if(factors.getLengthSq() <= by * by)
        *this = other;
    else {
        factors.normalize();
        *this += factors * by;
    }

    float intervalLen{interval.getLength()};

    x = std::fmod(x - interval.from, intervalLen);
    y = std::fmod(y - interval.from, intervalLen);
    z = std::fmod(z - interval.from, intervalLen);

    if(x < 0.f)
        x += intervalLen;

    if(y < 0.f)
        y += intervalLen;

    if(z < 0.f)
        z += intervalLen;

    x += interval.from;
    y += interval.from;
    z += interval.from;

    return *this;
}

template <typename T> inline Vec3 <T> &Vec3 <T>::move(T x, T y, T z)
{
    this->x += x;
    this->y += y;
    this->z += z;
    return *this;
}

template <typename T> inline Vec3 <T> Vec3 <T>::moved(T x, T y, T z) const
{
    return {this->x + x, this->y + y, this->z + z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::movedX(T x) const
{
    return {this->x + x, this->y, this->z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::movedY(T y) const
{
    return {this->x, this->y + y, this->z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::movedZ(T z) const
{
    return {this->x, this->y, this->z + z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::changedX(T x) const
{
    return {x, this->y, this->z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::changedY(T y) const
{
    return {this->x, y, this->z};
}

template <typename T> inline Vec3 <T> Vec3 <T>::changedZ(T z) const
{
    return {this->x, this->y, z};
}

template <typename T> inline bool Vec3 <T>::isFuzzyZero() const
{
    return Math::fuzzyCompare(x, static_cast <T> (0)) &&
           Math::fuzzyCompare(y, static_cast <T> (0)) &&
           Math::fuzzyCompare(z, static_cast <T> (0));
}

template <typename T> inline Vec3 <T> Vec3 <T>::lerped(const Vec3 &first, const Vec3 &second, float t)
{
    return {first.x + (second.x - first.x) * t,
            first.y + (second.y - first.y) * t,
            first.z + (second.z - first.z) * t};
}

template <typename T> inline Vec3 <T> Vec3 <T>::loopedLerped(const Vec3 &first, const Vec3 &second, float t, const FloatRange &interval)
{
    if(interval.isEmpty())
        return first;

    return {Math::loopedLerp(first.x, second.x, t, interval),
            Math::loopedLerp(first.y, second.y, t, interval),
            Math::loopedLerp(first.z, second.z, t, interval)};
}

} // namespace engine

#endif // ENGINE_VEC_3_HPP
