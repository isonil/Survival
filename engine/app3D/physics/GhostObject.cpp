#include "GhostObject.hpp"

#include "../Device.hpp"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace engine
{
namespace app3D
{

GhostObject::GhostObject(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset)
    : m_dynamicsWorld{dynamicsWorld},
      m_shape{shape},
      m_posOffset{posOffset}
{
    TRACK;

    if(m_dynamicsWorld.expired())
        throw Exception{"Dynamics world is nullptr."};

    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    btCollisionShape *btShape{&m_shape->getBtCollisionShape()};

    m_ghostObject = std::make_unique <btGhostObject> ();
    m_ghostObject->setCollisionShape(btShape);
    m_ghostObject->setCollisionFlags(m_ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    m_ghostObject->setUserIndex(-1);

    const auto &shared = m_dynamicsWorld.lock();

    E_DASSERT(shared, "Dynamics world is nullptr.");

    shared->addCollisionObject(m_ghostObject.get(), static_cast <short> (whatAmI), static_cast <short> (withWhatCollide));

    setPosition({});
}

void GhostObject::setPosition(const FloatVec3 &pos)
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &withOffset = pos + rotateAsGhostObject(m_posOffset);

    m_ghostObject->getWorldTransform().setOrigin({withOffset.x, withOffset.y, withOffset.z});
}

void GhostObject::setRotation(const FloatVec3 &rot)
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    auto prevPos = getPosition();

    btQuaternion quat;
    quat.setEulerZYX(Math::degToRad(rot.z), Math::degToRad(rot.y), Math::degToRad(rot.x));

    m_ghostObject->getWorldTransform().setRotation(quat);

    setPosition(prevPos);
}

FloatVec3 GhostObject::getPosition() const
{
    TRACK;

    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &origin = m_ghostObject->getWorldTransform().getOrigin();

    return FloatVec3{origin.getX(), origin.getY(), origin.getZ()} - rotateAsGhostObject(m_posOffset);
}

FloatVec3 GhostObject::getRotation() const
{
    TRACK;

    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &rotation = m_ghostObject->getWorldTransform().getRotation();

    irr::core::quaternion quat{rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW()};
    irr::core::vector3df eulerRadians;
    quat.toEuler(eulerRadians);

    return FloatVec3{Math::radToDeg(eulerRadians.X),
                           Math::radToDeg(eulerRadians.Y),
                           Math::radToDeg(eulerRadians.Z)};
}

btCollisionObject &GhostObject::getBtCollisionObject()
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    return *m_ghostObject;
}

GhostObject::~GhostObject()
{
    TRACK;

    const auto &shared = m_dynamicsWorld.lock();

    if(shared && m_ghostObject)
        shared->removeCollisionObject(m_ghostObject.get());
}

FloatVec3 GhostObject::rotateAsGhostObject(const FloatVec3 &vec) const
{
    const auto &rot = getRotation();

    btVector3 bVec{vec.x, vec.y, vec.z};

    bVec = bVec.rotate({1.f, 0.f, 0.f}, Math::degToRad(rot.x));
    bVec = bVec.rotate({0.f, 1.f, 0.f}, Math::degToRad(rot.y));
    bVec = bVec.rotate({0.f, 0.f, 1.f}, Math::degToRad(rot.z));

    return {bVec.x(), bVec.y(), bVec.z()};
}

} // namespace app3D
} // namespace engine
