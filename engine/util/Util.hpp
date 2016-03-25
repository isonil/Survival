#ifndef ENGINE_UTIL_HPP
#define ENGINE_UTIL_HPP

#include "Vec3.hpp"

namespace engine
{

class Util
{
public:
    static FloatVec3 getRandomPointOnSpherePart(const FloatVec3 &point, float maxAngle);
};

} // namespace engine

#endif // ENGINE_UTIL_HPP
