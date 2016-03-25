#ifndef ENGINE_APP_3D_PHYSICS_MANAGER_HPP
#define ENGINE_APP_3D_PHYSICS_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "../physics/CollisionFilter.hpp"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <memory>
#include <vector>

namespace engine { class AppTime; }

namespace engine
{
namespace app3D
{

class Device;
class RigidBody;
class GhostObject;
class CollisionDetector;
class CollisionShape;
class KinematicCharacterController;
class DynamicCharacterController;
class Ragdoll;
class Armature;
class Model;
class ConeTwistConstraint;

class PhysicsManager : public Tracked <PhysicsManager>
{
public:
    PhysicsManager(Device &device);

    void update(const AppTime &appTime);
    bool rayTest(const FloatVec3 &start, const FloatVec3 &end, CollisionFilter withWhatCollide, FloatVec3 &outPos, int &outHitBodyUserIndex);
    bool rayTest_notMe(const FloatVec3 &start, const FloatVec3 &end, const std::shared_ptr <RigidBody> &excludedBody, CollisionFilter withWhatCollide, FloatVec3 &outPos, int &outHitBodyUserIndex);

    btDynamicsWorld &getDynamicsWorld();

    std::shared_ptr <RigidBody> addRigidBody(const std::shared_ptr <CollisionShape> &shape, float mass, int userIndex = -1, const FloatVec3 &posOffset = {}, CollisionFilter additionalWhatAmIFlags = CollisionFilter::None);
    std::shared_ptr <GhostObject> addGhostObject(const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset = {});
    std::shared_ptr <CollisionDetector> addCollisionDetector(const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset = {});
    std::shared_ptr <KinematicCharacterController> addKinematicCharacterController(const std::shared_ptr <CollisionShape> &shape, float mass, const FloatVec3 &posOffset = {});
    std::shared_ptr <DynamicCharacterController> addDynamicCharacterController(const std::shared_ptr <CollisionShape> &shape, float mass, float heightToEyes, float waterLevel, bool canFly, int userIndex = -1, const FloatVec3 &posOffset = {});
    std::shared_ptr <Ragdoll> addRagdoll(const std::vector <std::shared_ptr <CollisionShape>> &collisionShapes, const std::vector <std::pair <int, int>> &constraints, int userIndex = -1);
    std::shared_ptr <Armature> addArmature(const Model &model);
    std::shared_ptr <ConeTwistConstraint> addConeTwistConstraint(const std::shared_ptr <RigidBody> &rigidBody1, const std::shared_ptr <RigidBody> &rigidBody2, const FloatVec3 &connectionPoint1, const FloatVec3 &connectionPoint2);

    static const float k_gravity;

private:
    void init();

    static const int k_maxPhysicsSubSteps;

    Device &m_device;

    std::unique_ptr <btBroadphaseInterface> m_broadphase;
    std::unique_ptr <btDefaultCollisionConfiguration> m_collisionConfiguration;
    std::unique_ptr <btCollisionDispatcher> m_collisionDispatcher;
    std::unique_ptr <btSequentialImpulseConstraintSolver> m_solver;
    std::shared_ptr <btDiscreteDynamicsWorld> m_dynamicsWorld;
    std::unique_ptr <btGhostPairCallback> m_ghostPairCallback;

    std::vector <std::shared_ptr <DynamicCharacterController>> m_dynamicCharacterControllers;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_PHYSICS_MANAGER_HPP
