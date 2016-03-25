#ifndef ENGINE_APP_3D_COLLISION_DETECTOR_HPP
#define ENGINE_APP_3D_COLLISION_DETECTOR_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"
#include "CollisionFilter.hpp"

#include <LinearMath/btAlignedObjectArray.h>

#include <memory>
#include <unordered_set>

class btPairCachingGhostObject;
class btPersistentManifold;

namespace engine
{
namespace app3D
{

class CollisionDetector : public Tracked <CollisionDetector>
{
public:
    CollisionDetector(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, CollisionFilter whoAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset);

    void setPosition(const FloatVec3 &pos);
    void setRotation(const FloatVec3 &rot);
    FloatVec3 getPosition() const;
    FloatVec3 getRotation() const;
    btCollisionObject &getBtCollisionObject();

    const std::unordered_set <int> &getCollidedObjectsUserIndices();

    ~CollisionDetector();

private:
    FloatVec3 rotateAsGhostObject(const FloatVec3 &vec) const;

    std::weak_ptr <btDynamicsWorld> m_dynamicsWorld;
    std::shared_ptr <CollisionShape> m_shape;
    std::unique_ptr <btPairCachingGhostObject> m_ghostObject;
    FloatVec3 m_posOffset;
    std::unordered_set <int> m_collidedObjectsUserIndices;
    btManifoldArray m_manifoldArray;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_COLLISION_DETECTOR_HPP
