#include "BvhTriangleMeshShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

BvhTriangleMeshShape::BvhTriangleMeshShape(const std::vector <FloatVec3> &trianglesPoints)
{
    TRACK;

    if(trianglesPoints.empty())
        throw Exception{"Can't construct bvh triangle mesh shape with 0 points."};

    if(trianglesPoints.size() % 3) {
        throw Exception{"Can't construct bvh triangle mesh shape because "
                              "triangles points count is not divisible by 3."};
    }

    for(size_t i = 0; i < trianglesPoints.size() / 3; ++i) {
        size_t index = i * 3;

        m_triangleMesh.addTriangle({trianglesPoints[index].x, trianglesPoints[index].y, trianglesPoints[index].z},
                                   {trianglesPoints[index + 1].x, trianglesPoints[index + 1].y, trianglesPoints[index + 1].z},
                                   {trianglesPoints[index + 2].x, trianglesPoints[index + 2].y, trianglesPoints[index + 2].z});
    }

    m_shape = std::make_unique <btBvhTriangleMeshShape> (&m_triangleMesh, false);
}

btCollisionShape &BvhTriangleMeshShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
