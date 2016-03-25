#include "Ragdoll.hpp"

#include "../managers/PhysicsManager.hpp"
#include "RigidBody.hpp"

#include <btBulletDynamicsCommon.h>

namespace engine
{
namespace app3D
{

Ragdoll::Ragdoll(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, PhysicsManager &physicsManager, const std::vector <std::shared_ptr <CollisionShape>> &collisionShapes, const std::vector <std::pair <int, int>> &constraints, int userIndex)
    : m_dynamicsWorld{dynamicsWorld}
{
    TRACK;

    const auto &shared = m_dynamicsWorld.lock();

    if(!shared)
        throw Exception{"Dynamics world is nullptr."};

    m_rigidBodies.reserve(collisionShapes.size());

    for(const auto &elem : collisionShapes) {
        m_rigidBodies.push_back(physicsManager.addRigidBody(elem, k_mass, userIndex));
    }

    for(const auto &elem : constraints) {
        int first{elem.first};
        int second{elem.second};

        if(first < 0 || static_cast <size_t> (first) >= m_rigidBodies.size())
            throw Exception{"Constraint body index out of bounds."};

        if(second < 0 || static_cast <size_t> (second) >= m_rigidBodies.size())
            throw Exception{"Constraint body index out of bounds."};

        E_DASSERT(m_rigidBodies[first], "Rigid body is nullptr.");
        E_DASSERT(m_rigidBodies[second], "Rigid body is nullptr.");

        auto &rigidBody1 = m_rigidBodies[first]->getBtRigidBody();
        auto &rigidBody2 = m_rigidBodies[second]->getBtRigidBody();

        btTransform t1{btQuaternion{0.f, 0.f, 0.f}};
        btTransform t2{btQuaternion{0.f, 0.f, 0.f}};

        m_constraints.push_back(std::make_unique <btConeTwistConstraint> (rigidBody1, rigidBody2, t1, t2));

        shared->addConstraint(m_constraints.back().get());
    }
}

void Ragdoll::setJointPosition(int index, const FloatVec3 &pos)
{
    if(index < 0 || static_cast <size_t> (index) >= m_rigidBodies.size())
        return;

    E_DASSERT(m_rigidBodies[index], "Rigid body is nullptr.");

    return m_rigidBodies[index]->setPosition(pos);
}

void Ragdoll::setJointRotation(int index, const FloatVec3 &rot)
{
    if(index < 0 || static_cast <size_t> (index) >= m_rigidBodies.size())
        return;

    E_DASSERT(m_rigidBodies[index], "Rigid body is nullptr.");

    return m_rigidBodies[index]->setRotation(rot);
}

FloatVec3 Ragdoll::getJointPosition(int index) const
{
    if(index < 0 || static_cast <size_t> (index) >= m_rigidBodies.size())
        return {};

    E_DASSERT(m_rigidBodies[index], "Rigid body is nullptr.");

    return m_rigidBodies[index]->getPosition();
}

FloatVec3 Ragdoll::getJointRotation(int index) const
{
    if(index < 0 || static_cast <size_t> (index) >= m_rigidBodies.size())
        return {};

    E_DASSERT(m_rigidBodies[index], "Rigid body is nullptr.");

    return m_rigidBodies[index]->getRotation();
}

Ragdoll::~Ragdoll()
{
    const auto &shared = m_dynamicsWorld.lock();

    if(shared) {
        for(const auto &elem : m_constraints) {
            E_DASSERT(elem, "Constraint is nullptr.");
            shared->removeConstraint(elem.get());
        }
    }
}

const float Ragdoll::k_mass{40.f};

} // namespace app3D
} // namespace engine
