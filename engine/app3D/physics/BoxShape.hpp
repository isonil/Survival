#ifndef ENGINE_APP_3D_BOX_SHAPE_HPP
#define ENGINE_APP_3D_BOX_SHAPE_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class BoxShape : public CollisionShape, public Tracked <BoxShape>
{
public:
    BoxShape(const FloatVec3 &size);

    btCollisionShape &getBtCollisionShape() override;

private:
    std::unique_ptr <btBoxShape> m_shape;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_BOX_SHAPE_HPP
