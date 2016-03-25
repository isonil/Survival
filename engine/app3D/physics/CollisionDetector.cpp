#include "CollisionDetector.hpp"

#include "../Device.hpp"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace engine
{
namespace app3D
{

CollisionDetector::CollisionDetector(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, CollisionFilter whatAmI, CollisionFilter withWhatCollide, const FloatVec3 &posOffset)
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

    m_ghostObject = std::make_unique <btPairCachingGhostObject> ();
    m_ghostObject->setCollisionShape(btShape);
    m_ghostObject->setCollisionFlags(m_ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    m_ghostObject->setUserIndex(-1);

    const auto &shared = m_dynamicsWorld.lock();

    E_DASSERT(shared, "Dynamics world is nullptr.");

    shared->addCollisionObject(m_ghostObject.get(), static_cast <short> (whatAmI), static_cast <short> (withWhatCollide));

    setPosition({});
}

void CollisionDetector::setPosition(const FloatVec3 &pos)
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &withOffset = pos + rotateAsGhostObject(m_posOffset);

    m_ghostObject->getWorldTransform().setOrigin({withOffset.x, withOffset.y, withOffset.z});
}

void CollisionDetector::setRotation(const FloatVec3 &rot)
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    auto prevPos = getPosition();

    btQuaternion quat;
    quat.setEulerZYX(Math::degToRad(rot.z), Math::degToRad(rot.y), Math::degToRad(rot.x));

    m_ghostObject->getWorldTransform().setRotation(quat);

    setPosition(prevPos);
}

FloatVec3 CollisionDetector::getPosition() const
{
    TRACK;

    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &origin = m_ghostObject->getWorldTransform().getOrigin();

    return FloatVec3{origin.getX(), origin.getY(), origin.getZ()} - rotateAsGhostObject(m_posOffset);
}

FloatVec3 CollisionDetector::getRotation() const
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

btCollisionObject &CollisionDetector::getBtCollisionObject()
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    return *m_ghostObject;
}

const std::unordered_set <int> &CollisionDetector::getCollidedObjectsUserIndices()
{
    /* TODO: implement a better way to get collided objects,
     * so it will be possible to check them midframe (currently it's updated by Bullet only once per frame)
     *
     * pseudocode:
     *  btGhostObject.getCollisionShape()->getAabb(btGhostObject.getWorldTransform(), aabbmin, aabbmax);
     *  Callback callback{m_ghostObject}; //in callback: btDynamicsWorld.contactTest(ghostObj, callback2);
     *  btDynamicsWorld.getBroadphase()->aabbTest(aabbmin, aabbmax, callback);
     *  return callback.getResults();
     *
     * (btPairCachingGhostObject to btGhostObject)
     */

    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    m_collidedObjectsUserIndices.clear();

    const auto &dynamicsWorld = m_dynamicsWorld.lock();
    E_DASSERT(dynamicsWorld, "Dynamics world is nullptr.");

    auto &pairArray = m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray();

    for(int i = 0; i < pairArray.size(); ++i) {
        const auto &pair = pairArray[i];

        auto *collisionPair = dynamicsWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

        if(!collisionPair)
            continue;

        m_manifoldArray.clear();

        if(collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(m_manifoldArray);

        for(int j = 0; j < m_manifoldArray.size(); ++j) {
            auto *manifold = m_manifoldArray[j];

            for(int p = 0; p < manifold->getNumContacts(); ++p) {
                const auto &pt = manifold->getContactPoint(p);

                if(pt.getDistance() < 0.f) {
                    if(manifold->getBody0() == m_ghostObject.get())
                        m_collidedObjectsUserIndices.insert(manifold->getBody1()->getUserIndex());
                    else if(manifold->getBody1() == m_ghostObject.get())
                        m_collidedObjectsUserIndices.insert(manifold->getBody0()->getUserIndex());
                }
            }
        }
    }

    return m_collidedObjectsUserIndices;
}

CollisionDetector::~CollisionDetector()
{
    TRACK;

    const auto &shared = m_dynamicsWorld.lock();

    if(shared && m_ghostObject)
        shared->removeCollisionObject(m_ghostObject.get());
}

FloatVec3 CollisionDetector::rotateAsGhostObject(const FloatVec3 &vec) const
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
