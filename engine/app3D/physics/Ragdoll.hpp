#ifndef ENGINE_APP_3D_RAGDOLL_HPP
#define ENGINE_APP_3D_RAGDOLL_HPP

#include "../../util/Vec3.hpp"
#include "../../util/Trace.hpp"

class btDynamicsWorld;
class btConeTwistConstraint;

namespace engine
{
namespace app3D
{

class RigidBody;
class CollisionShape;
class PhysicsManager;

// TODO: there should be a wrapper for different types of Bullet's constraints
// (and they should be passed to Ragdoll's constructor (maybe?))

// TODO: this class is probably wrong, it should probably use bones instead of rigid bodies for each joint

class Ragdoll : public Tracked <Ragdoll>
{
public:
    // TODO: we should be using already constructed RigidBodies here rather then creating them by ourselves using CollisionShapes
    Ragdoll(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, PhysicsManager &physicsManager, const std::vector <std::shared_ptr <CollisionShape>> &collisionShapes, const std::vector <std::pair <int, int>> &constraints, int userIndex);

    void setJointPosition(int index, const FloatVec3 &pos);
    void setJointRotation(int index, const FloatVec3 &rot);

    FloatVec3 getJointPosition(int index) const;
    FloatVec3 getJointRotation(int index) const;

    ~Ragdoll();

private:
    static const float k_mass;

    std::weak_ptr <btDynamicsWorld> m_dynamicsWorld;
    std::vector <std::shared_ptr <RigidBody>> m_rigidBodies;
    std::vector <std::unique_ptr <btConeTwistConstraint>> m_constraints;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_RAGDOLL_HPP

