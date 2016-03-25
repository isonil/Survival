#include "ConeTwistConstraint.hpp"

#include "RigidBody.hpp"

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btConeTwistConstraint.h>

namespace engine
{
namespace app3D
{

ConeTwistConstraint::ConeTwistConstraint(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <RigidBody> &rigidBody1, const std::shared_ptr <RigidBody> &rigidBody2, const FloatVec3 &connectionPoint1, const FloatVec3 &connectionPoint2)
    : m_dynamicsWorld{dynamicsWorld},
      m_rigidBody1{rigidBody1},
      m_rigidBody2{rigidBody2}
{
    const auto &dynamicsWorldShared = m_dynamicsWorld.lock();

    if(!dynamicsWorldShared)
        throw Exception{"Dynamics world is nullptr."};

    if(!m_rigidBody1)
        throw Exception{"Rigid body 1 is nullptr."};

    if(!m_rigidBody2)
        throw Exception{"Rigid body 2 is nullptr."};

    btTransform transform1;
    transform1.setIdentity();
    transform1.setOrigin({connectionPoint1.x, connectionPoint1.y, connectionPoint1.z});
    transform1.getBasis().setEulerZYX(0.f, Math::k_pi / 2.f, 0.f);

    btTransform transform2;
    transform2.setIdentity();
    transform2.setOrigin({connectionPoint2.x, connectionPoint2.y, connectionPoint2.z});
    transform2.getBasis().setEulerZYX(0.f, Math::k_pi / 2.f, 0.f);

    m_coneTwistConstraint = std::make_unique <btConeTwistConstraint> (m_rigidBody1->getBtRigidBody(), m_rigidBody2->getBtRigidBody(), transform1, transform2);
    m_coneTwistConstraint->setLimit(Math::k_pi / 2.f, Math::k_pi / 2.f);

    dynamicsWorldShared->addConstraint(m_coneTwistConstraint.get());
}

ConeTwistConstraint::~ConeTwistConstraint()
{
    TRACK;

    const auto &shared = m_dynamicsWorld.lock();

    if(shared && m_coneTwistConstraint)
        shared->removeConstraint(m_coneTwistConstraint.get());
}

} // namespace app3D
} // namespace engine
