#include "CapsuleShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

CapsuleShape::CapsuleShape(float radius, float height)
{
    if(radius < 0.f)
        throw Exception{"Tried to construct capsule shape using negative radius."};

    if(height < 0.f)
        throw Exception{"Tried to construct capsule shape using negative height."};

    m_shape = std::make_unique <btCapsuleShape> (radius, height);
}

btCollisionShape &CapsuleShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
