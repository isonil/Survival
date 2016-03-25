#include "SphereShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

SphereShape::SphereShape(float radius)
{
    if(radius < 0.f)
        throw Exception{"Tried to construct sphere shape using negative radius."};

    m_shape = std::make_unique <btSphereShape> (radius);
}

btCollisionShape &SphereShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
