#include "ConvexHullShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

ConvexHullShape::ConvexHullShape(const std::vector <FloatVec3> &points)
    : m_shape{std::make_unique <btConvexHullShape> ()}
{
    TRACK;

    if(points.empty())
        throw Exception{"Can't construct convex hull shape with 0 points."};

    for(const auto &elem : points) {
        m_shape->addPoint({elem.x, elem.y, elem.z}, false);
    }

    m_shape->recalcLocalAabb();
}

btCollisionShape &ConvexHullShape::getBtCollisionShape()
{
    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    return *m_shape;
}

} // namespace app3D
} // namespace engine
