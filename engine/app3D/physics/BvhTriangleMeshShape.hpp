#ifndef ENGINE_APP_3D_BVH_TRIANGLE_MESH_SHAPE_HPP
#define ENGINE_APP_3D_BVH_TRIANGLE_MESH_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class BvhTriangleMeshShape : public CollisionShape, public Tracked <BvhTriangleMeshShape>
{
public:
    BvhTriangleMeshShape(const std::vector <FloatVec3> &trianglesPoints);

    btCollisionShape &getBtCollisionShape() override;

private:
    btTriangleMesh m_triangleMesh;
    std::unique_ptr <btBvhTriangleMeshShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_BVH_TRIANGLE_MESH_SHAPE_HPP
