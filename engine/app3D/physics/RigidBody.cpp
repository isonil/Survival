#include "RigidBody.hpp"

#include "../Device.hpp"

namespace engine
{
namespace app3D
{

RigidBody::RigidBody(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, float mass, int userIndex, const FloatVec3 &posOffset, CollisionFilter additionalWhatAmIFlags)
    : m_dynamicsWorld{dynamicsWorld},
      m_shape{shape},
      m_mass{mass},
      m_userIndex(userIndex),
      m_posOffset{posOffset}
{
    TRACK;

    if(m_dynamicsWorld.expired())
        throw Exception{"Dynamics world is nullptr."};

    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    if(m_mass < 0.f)
        throw Exception{"Mass can't be negative."};

    m_motionState = std::make_unique <btDefaultMotionState> (btTransform{btQuaternion{0.f, 0.f, 0.f, 1.f}, btVector3{0.f, 0.f, 0.f}});

    btVector3 fallInertia{0.f, 0.f, 0.f};

    btCollisionShape *btShape{&m_shape->getBtCollisionShape()};

    btShape->calculateLocalInertia(m_mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(m_mass, m_motionState.get(), btShape, fallInertia);

    m_rigidBody = std::make_unique <btRigidBody> (rigidBodyCI);
    m_rigidBody->setUserIndex(m_userIndex);

    const auto &shared = m_dynamicsWorld.lock();

    E_DASSERT(shared, "Dynamics world is nullptr.");

    shared->addRigidBody(
        m_rigidBody.get(),
        static_cast <short int> (getCollisionGroupForBtRigidBody(*m_rigidBody, additionalWhatAmIFlags)),
        static_cast <short int> (getCollisionMaskForBtRigidBody(*m_rigidBody)));

    setPosition({});
}

void RigidBody::setPosition(const FloatVec3 &pos)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    const auto &withOffset = pos + rotateAsBody(m_posOffset);

    btVector3 bWithOffset{withOffset.x, withOffset.y, withOffset.z};

    btTransform transform{m_rigidBody->getCenterOfMassTransform()};
    transform.setOrigin(bWithOffset);
    m_rigidBody->setCenterOfMassTransform(transform);

    auto *motionState = m_rigidBody->getMotionState();

    motionState->getWorldTransform(transform);
    transform.setOrigin(bWithOffset);
    motionState->setWorldTransform(transform);
}

void RigidBody::setRotation(const FloatVec3 &rot)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    auto prevPos = getPosition();

    btQuaternion quat;
    quat.setEulerZYX(Math::degToRad(rot.z), Math::degToRad(rot.y), Math::degToRad(rot.x));

    btTransform transform{m_rigidBody->getCenterOfMassTransform()};
    transform.setRotation(quat);
    m_rigidBody->setCenterOfMassTransform(transform);

    auto *motionState = m_rigidBody->getMotionState();

    motionState->getWorldTransform(transform);
    transform.setRotation(quat);
    motionState->setWorldTransform(transform);

    setPosition(prevPos);
}

void RigidBody::setLinearVelocity(const FloatVec3 &vel)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setLinearVelocity({vel.x, vel.y, vel.z});
}

void RigidBody::setAngularVelocity(const FloatVec3 &vel)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setAngularVelocity({vel.x, vel.y, vel.z});
}

void RigidBody::setDamping(float damping)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setDamping(damping, damping);
}

void RigidBody::setFriction(float friction, float rollingFriction)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setFriction(friction);
    m_rigidBody->setRollingFriction(rollingFriction);
}

void RigidBody::setGravity(float gravity)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setGravity({0.f, gravity, 0.f});
}

void RigidBody::applyForce(const FloatVec3 &force, const FloatVec3 &relPos)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->applyForce({force.x, force.y, force.z}, {relPos.x, relPos.y, relPos.z});
}

void RigidBody::lockFallingOver()
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setAngularFactor({0.f, 0.f, 0.f});
}

void RigidBody::disableDeactivationState()
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

btCollisionObject &RigidBody::getBtCollisionObject()
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return *m_rigidBody;
}

FloatVec3 RigidBody::getPosition() const
{
    TRACK;

    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    btTransform transform;

    m_rigidBody->getMotionState()->getWorldTransform(transform);

    const auto &origin = transform.getOrigin();

    return FloatVec3{origin.getX(), origin.getY(), origin.getZ()} - rotateAsBody(m_posOffset);
}

FloatVec3 RigidBody::getRotation() const
{
    TRACK;

    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    btTransform transform;

    m_rigidBody->getMotionState()->getWorldTransform(transform);

    const auto &rotation = transform.getRotation();

    irr::core::quaternion quat{rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW()};
    irr::core::vector3df eulerRadians;
    quat.toEuler(eulerRadians);

    return FloatVec3{Math::radToDeg(eulerRadians.X),
                           Math::radToDeg(eulerRadians.Y),
                           Math::radToDeg(eulerRadians.Z)};
}

FloatVec3 RigidBody::getLinearVelocity() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    const auto &vel = m_rigidBody->getLinearVelocity();

    return FloatVec3{vel.getX(), vel.getY(), vel.getZ()};
}

FloatVec3 RigidBody::getAngularVelocity() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    const auto &vel = m_rigidBody->getAngularVelocity();

    return FloatVec3{vel.getX(), vel.getY(), vel.getZ()};
}

float RigidBody::getAABBHeight() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    btVector3 min{}, max{};

    m_rigidBody->getAabb(min, max);

    return std::fabs(max.y() - min.y());
}

btRigidBody &RigidBody::getBtRigidBody() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return *m_rigidBody;
}

void RigidBody::affectByWater(float waterLevel)
{
    float posY{getPosition().y};

    if(posY < waterLevel) {
        applyForce({0.f, (waterLevel - posY) * 100.f, 0.f}, {});
        setDamping(0.95f);
    }
    else
        setDamping(0.f);
}

RigidBody::~RigidBody()
{
    TRACK;

    const auto &shared = m_dynamicsWorld.lock();

    if(shared && m_rigidBody)
        shared->removeRigidBody(m_rigidBody.get());
}

FloatVec3 RigidBody::rotateAsBody(const FloatVec3 &vec) const
{
    const auto &rot = getRotation();

    btVector3 bVec{vec.x, vec.y, vec.z};

    bVec = bVec.rotate({1.f, 0.f, 0.f}, Math::degToRad(rot.x));
    bVec = bVec.rotate({0.f, 1.f, 0.f}, Math::degToRad(rot.y));
    bVec = bVec.rotate({0.f, 0.f, 1.f}, Math::degToRad(rot.z));

    return {bVec.x(), bVec.y(), bVec.z()};
}

CollisionFilter RigidBody::getCollisionGroupForBtRigidBody(const btRigidBody &body, CollisionFilter additionalWhatAmIFlags) const
{
    // the only thing that's different from Bullet's behavior is our additional flags

    auto isDynamic = !(body.isStaticObject() || body.isKinematicObject());

    CollisionFilter group = isDynamic ? CollisionFilter::Default : CollisionFilter::Static;

    return group | additionalWhatAmIFlags;
}

CollisionFilter RigidBody::getCollisionMaskForBtRigidBody(const btRigidBody &body) const
{
    // this is basically the same what Bullet does if we don't specify custom collision mask,
    // but since we do (for groups), we have to calculate it too

    auto isDynamic = !(body.isStaticObject() || body.isKinematicObject());

    CollisionFilter mask = isDynamic ? CollisionFilter::All : (CollisionFilter::All ^ CollisionFilter::Static);

    return mask;
}

} // namespace app3D
} // namespace engine
