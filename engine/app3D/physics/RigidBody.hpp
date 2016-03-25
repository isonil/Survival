#ifndef ENGINE_APP_3D_RIGID_BODY_HPP
#define ENGINE_APP_3D_RIGID_BODY_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"
#include "CollisionFilter.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class RigidBody : public Tracked <RigidBody>
{
public:
    RigidBody(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, float mass, int userIndex, const FloatVec3 &posOffset, CollisionFilter additionalWhatAmIFlags);

    void setPosition(const FloatVec3 &pos);
    void setRotation(const FloatVec3 &rot);
    void setLinearVelocity(const FloatVec3 &vel);
    void setAngularVelocity(const FloatVec3 &vel);
    void setDamping(float damping);
    void setFriction(float friction, float rollingFriction);
    void setGravity(float gravity);
    void applyForce(const FloatVec3 &force, const FloatVec3 &relPos);
    void lockFallingOver();
    void disableDeactivationState();
    btCollisionObject &getBtCollisionObject();
    FloatVec3 getPosition() const;
    FloatVec3 getRotation() const;
    FloatVec3 getLinearVelocity() const;
    FloatVec3 getAngularVelocity() const;
    float getAABBHeight() const;
    btRigidBody &getBtRigidBody() const;

    void affectByWater(float waterLevel);

    ~RigidBody();

private:
    FloatVec3 rotateAsBody(const FloatVec3 &vec) const;
    CollisionFilter getCollisionGroupForBtRigidBody(const btRigidBody &body, CollisionFilter additionalWhatAmIFlags) const;
    CollisionFilter getCollisionMaskForBtRigidBody(const btRigidBody &body) const;

    std::weak_ptr <btDynamicsWorld> m_dynamicsWorld;
    std::shared_ptr <CollisionShape> m_shape;
    std::unique_ptr <btDefaultMotionState> m_motionState;
    std::unique_ptr <btRigidBody> m_rigidBody;
    float m_mass;
    int m_userIndex;
    FloatVec3 m_posOffset;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_RIGID_BODY_HPP
