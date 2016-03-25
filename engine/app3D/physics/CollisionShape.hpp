#ifndef ENGINE_APP_3D_COLLISION_SHAPE_HPP
#define ENGINE_APP_3D_COLLISION_SHAPE_HPP

#include "../../util/Trace.hpp"

#include <btBulletDynamicsCommon.h>

namespace engine
{
namespace app3D
{

class CollisionShape : public Tracked <CollisionShape>
{
public:
    virtual btCollisionShape &getBtCollisionShape() = 0;

    virtual ~CollisionShape() = default;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_COLLISION_SHAPE_HPP
