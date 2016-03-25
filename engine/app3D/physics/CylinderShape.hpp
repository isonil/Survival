#ifndef ENGINE_APP_3D_CYLINDER_SHAPE_HPP
#define ENGINE_APP_3D_CYLINDER_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class CylinderShape : public CollisionShape, public Tracked <CylinderShape>
{
public:
    CylinderShape(const FloatVec3 &size);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btCylinderShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_CYLINDER_SHAPE_HPP
