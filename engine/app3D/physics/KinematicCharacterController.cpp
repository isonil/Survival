#include "KinematicCharacterController.hpp"

#include "../Device.hpp"

namespace engine
{
namespace app3D
{

KinematicCharacterController::KinematicCharacterController(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, float mass, const FloatVec3 &posOffset)
    : m_dynamicsWorld{dynamicsWorld},
      m_shape{shape},
      m_mass(mass),
      m_posOffset{posOffset}
{
    TRACK;

    if(m_dynamicsWorld.expired())
        throw Exception{"Dynamics world is nullptr."};

    if(!m_shape)
        throw Exception{"Shape is nullptr."};

    if(m_mass < 0.f)
        throw Exception{"Mass can't be negative."};

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin({0.f, 0.f, 0.f});

    m_ghostObject = std::make_unique <btPairCachingGhostObject> ();
    m_ghostObject->setWorldTransform(startTransform);
    m_ghostObject->setCollisionShape(&m_shape->getBtCollisionShape());
    m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    m_ghostObject->setUserIndex(-1);

    auto *convexShape = dynamic_cast <btConvexShape*> (&m_shape->getBtCollisionShape());

    if(!convexShape)
        throw Exception{"Character's shape must be convex."};

    m_character = std::make_unique <btKinematicCharacterController> (m_ghostObject.get(), convexShape, k_stepHeight);

    const auto &shared = m_dynamicsWorld.lock();

    E_DASSERT(shared, "Dynamics world is nullptr.");

    shared->addCollisionObject(m_ghostObject.get(), btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    shared->addAction(m_character.get());
}

void KinematicCharacterController::setPosition(const FloatVec3 &pos)
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &withOffset = pos + m_posOffset;

    btTransform transform{m_ghostObject->getWorldTransform()};
    transform.setOrigin({withOffset.x, withOffset.y, withOffset.z});

    m_ghostObject->setWorldTransform(transform);
}

void KinematicCharacterController::setMovement(const FloatVec3 &movement)
{
    E_DASSERT(m_character, "Character is nullptr.");

    auto movement2 = movement;

    // HACK: Bullet Physics bug: walk direction can't be 0 (yes, it's a bug; it's not really 'direction')
    // (it's the only way to stop movement)
    if(movement2.getLength() == 0.f)
        movement2.x = 0.00000001f;

    m_character->setWalkDirection({movement2.x, movement2.y, movement2.z});
}

FloatVec3 KinematicCharacterController::getPosition() const
{
    E_DASSERT(m_ghostObject, "Ghost object is nullptr.");

    const auto &origin = m_ghostObject->getWorldTransform().getOrigin();

    return FloatVec3{origin.getX(), origin.getY(), origin.getZ()} - m_posOffset;
}

KinematicCharacterController::~KinematicCharacterController()
{
    TRACK;

    const auto &shared = m_dynamicsWorld.lock();

    if(shared) {
        if(m_character)
            shared->removeAction(m_character.get());

        if(m_ghostObject)
            shared->removeCollisionObject(m_ghostObject.get());
    }
}

const float KinematicCharacterController::k_stepHeight{0.35f};

} // namespace app3D
} // namespace engine
