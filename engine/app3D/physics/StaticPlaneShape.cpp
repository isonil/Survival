#include "StaticPlaneShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

StaticPlaneShape::StaticPlaneShape(const FloatVec3 &planeNormal, float planeConstant)
{
    if(planeNormal.isFuzzyZero())
        throw Exception{"Tried to construct static plane shape using zero normal vector."};

    m_shape = std::make_unique <btStaticPlaneShape> (btVector3{planeNormal.x, planeNormal.y, planeNormal.z}, planeConstant);
}

btCollisionShape &StaticPlaneShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
