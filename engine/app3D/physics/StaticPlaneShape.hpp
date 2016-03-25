#ifndef ENGINE_APP_3D_STATIC_PLANE_SHAPE_HPP
#define ENGINE_APP_3D_STATIC_PLANE_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class StaticPlaneShape : public CollisionShape, public Tracked <StaticPlaneShape>
{
public:
    StaticPlaneShape(const FloatVec3 &planeNormal, float planeConstant);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btStaticPlaneShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_STATIC_PLANE_SHAPE_HPP
