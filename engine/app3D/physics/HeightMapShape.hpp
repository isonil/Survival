#ifndef ENGINE_APP_3D_HEIGHT_MAP_SHAPE_HPP
#define ENGINE_APP_3D_HEIGHT_MAP_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <memory>

namespace engine
{
namespace app3D
{

class HeightMapShape : public CollisionShape, public Tracked <HeightMapShape>
{
public:
    HeightMapShape(const std::vector <float> &heightMapHeightValues);

    btCollisionShape &getBtCollisionShape() override;

    void setLocalScaling(const FloatVec3 &scaling);
    float getMeshSideSize() const;
    float getHeight() const;

private:
    std::unique_ptr <btHeightfieldTerrainShape> m_shape;
    std::vector <float> m_heightMapHeightValues;
    float m_meshSideSize;
    float m_height;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_HEIGHT_MAP_SHAPE_HPP
