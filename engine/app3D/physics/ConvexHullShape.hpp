#ifndef ENGINE_APP_3D_CONVEX_HULL_SHAPE_HPP
#define ENGINE_APP_3D_CONVEX_HULL_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class ConvexHullShape : public CollisionShape, public Tracked <ConvexHullShape>
{
public:
    ConvexHullShape(const std::vector <FloatVec3> &points);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btConvexHullShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_CONVEX_HULL_SHAPE_HPP
