#include "HeightMapShape.hpp"

#include "../../util/Exception.hpp"

namespace engine
{
namespace app3D
{

HeightMapShape::HeightMapShape(const std::vector <float> &heightMapHeightValues)
    : m_heightMapHeightValues{heightMapHeightValues},
      m_meshSideSize{},
      m_height{}
{
    TRACK;

    if(m_heightMapHeightValues.empty())
        throw Exception{"Can't construct height map shape with 0 height map height values."};

    int size{};

    for(int i = 1; i < static_cast <int> (m_heightMapHeightValues.size()) / 2 + 1; ++i) {
        if(i * i == static_cast <int> (m_heightMapHeightValues.size())) {
            size = i;
            break;
        }
    }

    if(!size)
        throw Exception{"Height map height values size must be x * x."};

    m_meshSideSize = size;

    btScalar maxHeight{m_heightMapHeightValues[0]};

    for(const auto &elem : m_heightMapHeightValues) {
        if(elem < 0.f)
            throw Exception{"Height map height value is negative."};

        if(elem > maxHeight)
            maxHeight = elem;
    }

    m_height = maxHeight;

    m_shape = std::make_unique <btHeightfieldTerrainShape> (size, size, &m_heightMapHeightValues[0], 1.f, 0.f, maxHeight, 1, PHY_FLOAT, false);
}

btCollisionShape &HeightMapShape::getBtCollisionShape()
{
    E_DASSERT(m_shape, "Shape is nullptr.");

    return *m_shape;
}

void HeightMapShape::setLocalScaling(const FloatVec3 &scaling)
{
    E_DASSERT(m_shape, "Shape is nullptr.");

    m_shape->setLocalScaling({scaling.x, scaling.y, scaling.z});
}

float HeightMapShape::getMeshSideSize() const
{
    return m_meshSideSize;
}

float HeightMapShape::getHeight() const
{
    return m_height;
}

} // namespace app3D
} // namespace engine
