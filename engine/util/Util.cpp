#include "Util.hpp"

#include "Random.hpp"

#include <irrlicht.h>

namespace engine
{

FloatVec3 Util::getRandomPointOnSpherePart(const FloatVec3 &point, float maxAngle)
{
    if(maxAngle <= 0.f)
        return point;

    float angle{Random::rangeInclusive(0.f, maxAngle)};

    irr::core::vector3df irrPoint{point.x, point.y, point.z};

    // first, we need a random normalized vector

    irr::core::vector3df randomNormalized;

    while(randomNormalized.getLength() < 0.001f) {
        randomNormalized.set(
            Random::nextGaussian(),
            Random::nextGaussian(),
            Random::nextGaussian()
        );
    }

    randomNormalized.normalize();

    // now we calculate cross product with our point, and we get random orthogonal normalized vector

    irr::core::vector3df randomOrthogonal = irrPoint.crossProduct(randomNormalized);
    randomOrthogonal.normalize();

    // here we rotate irrPoint around randomOrthogonal axis by angle degrees

    irr::core::quaternion quat;
    quat.fromAngleAxis(Math::degToRad(angle), randomOrthogonal);

    irr::core::matrix4 mat;
    quat.getMatrix(mat, {});

    auto result = irrPoint;
    mat.rotateVect(result);

    // and this is our result

    return {result.X, result.Y, result.Z};
}

} // namespace engine
