#ifndef ENGINE_APP_3D_GHOST_OBJECT_HPP
#define ENGINE_APP_3D_GHOST_OBJECT_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"
#include "CollisionFilter.hpp"

#include <memory>

class btGhostObject;

namespace engine
{
namespace app3D
{

class GhostObject : public Tracked <GhostObject>
{
public:
    GhostObject(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset);

    void setPosition(const FloatVec3 &pos);
    void setRotation(const FloatVec3 &rot);
    FloatVec3 getPosition() const;
    FloatVec3 getRotation() const;
    btCollisionObject &getBtCollisionObject();

    ~GhostObject();

private:
    FloatVec3 rotateAsGhostObject(const FloatVec3 &vec) const;

    std::weak_ptr <btDynamicsWorld> m_dynamicsWorld;
    std::shared_ptr <CollisionShape> m_shape;
    std::unique_ptr <btGhostObject> m_ghostObject;
    FloatVec3 m_posOffset;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_GHOST_OBJECT_HPP
