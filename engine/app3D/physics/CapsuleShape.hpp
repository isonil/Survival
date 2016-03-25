#ifndef ENGINE_APP_3D_CAPSULE_SHAPE_HPP
#define ENGINE_APP_3D_CAPSULE_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class CapsuleShape : public CollisionShape, public Tracked <CapsuleShape>
{
public:
    CapsuleShape(float radius, float height);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btCapsuleShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_CAPSULE_SHAPE_HPP
