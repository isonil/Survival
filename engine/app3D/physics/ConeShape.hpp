#ifndef ENGINE_APP_3D_CONE_SHAPE_HPP
#define ENGINE_APP_3D_CONE_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class ConeShape : public CollisionShape, public Tracked <ConeShape>
{
public:
    ConeShape(float radius, float height);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btConeShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_CONE_SHAPE_HPP
