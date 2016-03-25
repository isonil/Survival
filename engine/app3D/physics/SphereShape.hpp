#ifndef ENGINE_APP_3D_SPHERE_SHAPE_HPP
#define ENGINE_APP_3D_SPHERE_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class SphereShape : public CollisionShape, public Tracked <SphereShape>
{
public:
    SphereShape(float radius);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btSphereShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_SPHERE_SHAPE_HPP
