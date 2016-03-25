#include "CylinderShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

CylinderShape::CylinderShape(const FloatVec3 &size)
{
    if(size.x < 0.f || size.y < 0.f || size.z < 0.f)
        throw Exception{"Tried to construct cylinder shape using negative size."};

    m_shape = std::make_unique <btCylinderShape> (btVector3{size.x * 0.5f, size.y * 0.5f, size.z * 0.5f});
}

btCollisionShape &CylinderShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
