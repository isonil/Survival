#include "BoxShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

BoxShape::BoxShape(const FloatVec3 &size)
{
    if(size.x < 0.f || size.y < 0.f || size.z < 0.f)
        throw Exception{"Tried to construct box shape using negative size."};

    m_shape = std::make_unique <btBoxShape> (btVector3{size.x * 0.5f, size.y * 0.5f, size.z * 0.5f});
}

btCollisionShape &BoxShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
