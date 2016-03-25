#include "PhysicsManager.hpp"

#include "../../util/AppTime.hpp"
#include "../physics/KinematicCharacterController.hpp"
#include "../physics/DynamicCharacterController.hpp"
#include "../physics/CollisionShape.hpp"
#include "../physics/RigidBody.hpp"
#include "../physics/GhostObject.hpp"
#include "../physics/CollisionDetector.hpp"
#include "../physics/Ragdoll.hpp"
#include "../physics/Armature.hpp"
#include "../physics/ConeTwistConstraint.hpp"
#include "../Device.hpp"

namespace engine
{
namespace app3D
{

class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
    ClosestNotMeRayResultCallback(const btVector3 &from, const btVector3 &to, btCollisionObject &me)
        : btCollisionWorld::ClosestRayResultCallback{from, to},
          m_me{me}
    {
    }

    btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) override
    {
        if(rayResult.m_collisionObject == &m_me)
            return 1.f;

        return base::addSingleResult(rayResult, normalInWorldSpace);
    }

private:
    typedef btCollisionWorld::ClosestRayResultCallback base;

    btCollisionObject &m_me;
};

PhysicsManager::PhysicsManager(Device &device)
    : m_device{device}
{
    init();
}

void PhysicsManager::update(const AppTime &appTime)
{
    TRACK;

    for(size_t i = 0; i < m_dynamicCharacterControllers.size();) {
        if(m_dynamicCharacterControllers[i].unique()) {
            std::swap(m_dynamicCharacterControllers[i], m_dynamicCharacterControllers.back());
            m_dynamicCharacterControllers.pop_back();
        }
        else {
            E_DASSERT(m_dynamicCharacterControllers[i], "Dynamic character controller is nullptr.");
            m_dynamicCharacterControllers[i]->update(*this);
            ++i;
        }
    }

    E_DASSERT(m_dynamicsWorld, "Dynamics world is nullptr.");

    m_dynamicsWorld->stepSimulation(appTime.getDeltaAsSeconds(), k_maxPhysicsSubSteps);
}

bool PhysicsManager::rayTest(const FloatVec3 &start, const FloatVec3 &end, CollisionFilter withWhatCollide, FloatVec3 &outPos, int &outHitBodyUserIndex)
{
    outPos.set(0.f, 0.f, 0.f);
    outHitBodyUserIndex = -1;

    E_DASSERT(m_dynamicsWorld, "Dynamics world is nullptr.");

    btVector3 btFrom{start.x, start.y, start.z};
    btVector3 btTo{end.x, end.y, end.z};
    btCollisionWorld::ClosestRayResultCallback resultCallback{btFrom, btTo};

    resultCallback.m_collisionFilterGroup = static_cast <short> (CollisionFilter::Raycast);
    resultCallback.m_collisionFilterMask = static_cast <short> (withWhatCollide);

    m_dynamicsWorld->rayTest(btFrom, btTo, resultCallback);

    if(resultCallback.hasHit()) {
        const auto &btPos = resultCallback.m_hitPointWorld;
        outPos = FloatVec3{btPos.x(), btPos.y(), btPos.z()};

        const auto *hitBody = resultCallback.m_collisionObject;
        if(hitBody)
            outHitBodyUserIndex = hitBody->getUserIndex();

        return true;
    }

    return false;
}

bool PhysicsManager::rayTest_notMe(const FloatVec3 &start, const FloatVec3 &end, const std::shared_ptr <RigidBody> &excludedBody, CollisionFilter withWhatCollide, FloatVec3 &outPos, int &outHitBodyUserIndex)
{
    if(!excludedBody)
        return rayTest(start, end, withWhatCollide, outPos, outHitBodyUserIndex);

    outPos.set(0.f, 0.f, 0.f);
    outHitBodyUserIndex = -1;

    E_DASSERT(m_dynamicsWorld, "Dynamics world is nullptr.");

    btVector3 btFrom{start.x, start.y, start.z};
    btVector3 btTo{end.x, end.y, end.z};

    ClosestNotMeRayResultCallback resultCallback{btFrom, btTo, excludedBody->getBtCollisionObject()};

    resultCallback.m_collisionFilterGroup = static_cast <short> (CollisionFilter::Raycast);
    resultCallback.m_collisionFilterMask = static_cast <short> (withWhatCollide);

    m_dynamicsWorld->rayTest(btFrom, btTo, resultCallback);

    if(resultCallback.hasHit()) {
        const auto &btPos = resultCallback.m_hitPointWorld;
        outPos = FloatVec3{btPos.x(), btPos.y(), btPos.z()};

        const auto *hitBody = resultCallback.m_collisionObject;
        if(hitBody)
            outHitBodyUserIndex = hitBody->getUserIndex();

        return true;
    }

    return false;
}

btDynamicsWorld &PhysicsManager::getDynamicsWorld()
{
    if(!m_dynamicsWorld)
        throw Exception{"Dynamics world is nullptr."};

    return *m_dynamicsWorld;
}

std::shared_ptr <RigidBody> PhysicsManager::addRigidBody(const std::shared_ptr <CollisionShape> &shape, float mass, int userIndex, const FloatVec3 &posOffset, CollisionFilter additionalWhatAmIFlags)
{
    const auto &rigidBody = std::make_shared <RigidBody> (m_dynamicsWorld, shape, mass, userIndex, posOffset, additionalWhatAmIFlags);
    return rigidBody;
}

std::shared_ptr <GhostObject> PhysicsManager::addGhostObject(const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset)
{
    const auto &ghostObject = std::make_shared <GhostObject> (m_dynamicsWorld, shape, whatAmI, withWhatCollide, posOffset);
    return ghostObject;
}

std::shared_ptr <CollisionDetector> PhysicsManager::addCollisionDetector(const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset)
{
    const auto &collisionDetector = std::make_shared <CollisionDetector> (m_dynamicsWorld, shape, whatAmI, withWhatCollide, posOffset);
    return collisionDetector;
}

std::shared_ptr <KinematicCharacterController> PhysicsManager::addKinematicCharacterController(const std::shared_ptr <CollisionShape> &shape, float mass, const FloatVec3 &posOffset)
{
    const auto &character = std::make_shared <KinematicCharacterController> (m_dynamicsWorld, shape, mass, posOffset);
    return character;
}

std::shared_ptr <DynamicCharacterController> PhysicsManager::addDynamicCharacterController(const std::shared_ptr <CollisionShape> &shape, float mass, float heightToEyes, float waterLevel, bool canFly, int userIndex, const FloatVec3 &posOffset)
{
    const auto &rigidBody = addRigidBody(shape, mass, userIndex, posOffset, CollisionFilter::Character);
    const auto &character = std::make_shared <DynamicCharacterController> (rigidBody, heightToEyes, waterLevel, canFly);

    m_dynamicCharacterControllers.push_back(character);

    return character;
}

std::shared_ptr <Ragdoll> PhysicsManager::addRagdoll(const std::vector <std::shared_ptr <CollisionShape>> &collisionShapes, const std::vector <std::pair <int, int>> &constraints, int userIndex)
{
    const auto &ragdoll = std::make_shared <Ragdoll> (m_dynamicsWorld, *this, collisionShapes, constraints, userIndex);
    return ragdoll;
}

std::shared_ptr <Armature> PhysicsManager::addArmature(const Model &model)
{
    const auto &armature = std::make_shared <Armature> (*this, model);
    return armature;
}

std::shared_ptr <ConeTwistConstraint> PhysicsManager::addConeTwistConstraint(const std::shared_ptr <RigidBody> &rigidBody1, const std::shared_ptr <RigidBody> &rigidBody2, const FloatVec3 &connectionPoint1, const FloatVec3 &connectionPoint2)
{
    const auto &coneTwistConstraint = std::make_shared <ConeTwistConstraint> (m_dynamicsWorld, rigidBody1, rigidBody2, connectionPoint1, connectionPoint2);
    return coneTwistConstraint;
}

const float PhysicsManager::k_gravity{-9.81f};

void PhysicsManager::init()
{
    m_ghostPairCallback = std::make_unique <btGhostPairCallback> ();
    m_broadphase = std::make_unique <btDbvtBroadphase> ();
    m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback.get());
    m_collisionConfiguration = std::make_unique <btDefaultCollisionConfiguration> ();
    m_collisionDispatcher = std::make_unique <btCollisionDispatcher> (m_collisionConfiguration.get());
    m_solver = std::make_unique <btSequentialImpulseConstraintSolver> ();

    m_dynamicsWorld = std::make_shared <btDiscreteDynamicsWorld> (m_collisionDispatcher.get(),
        m_broadphase.get(),
        m_solver.get(),
        m_collisionConfiguration.get());

    m_dynamicsWorld->setGravity({0.f, k_gravity, 0.f});
}

const int PhysicsManager::k_maxPhysicsSubSteps{5};

} // namespace app3D
} // namespace engine
