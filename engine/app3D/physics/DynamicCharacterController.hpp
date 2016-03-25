#ifndef ENGINE_APP_3D_DYNAMIC_CHARACTER_CONTROLLER_HPP
#define ENGINE_APP_3D_DYNAMIC_CHARACTER_CONTROLLER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionFilter.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class PhysicsManager;
class RigidBody;

class DynamicCharacterController : public Tracked <DynamicCharacterController>
{
public:
    DynamicCharacterController(const std::shared_ptr <RigidBody> &rigidBody, float heightToEyes, float waterLevel, bool canFly);

    void update(PhysicsManager &physicsManager);

    bool wasWalkingPreviousFrame() const;
    bool wasTryingToMovePreviousFrame() const;
    int getBodyUserIndexOnWhichWalkedPreviousFrame() const;
    bool isOnGround() const;
    bool canJump() const;
    void jump();
    void setMovement(const FloatVec3 &movement);
    void setPosition(const FloatVec3 &pos);
    const FloatVec3 &getMovement() const;
    FloatVec3 getPosition() const;
    FloatVec3 getLinearVelocity() const;
    bool rayTest_notMe(PhysicsManager &physicsManager, const FloatVec3 &start, const FloatVec3 &end, CollisionFilter withWhatCollide, FloatVec3 &outPos, int &outHitBodyUserIndex);
    bool isSwimming() const;
    bool isUnderWater() const;
    void setJumpVelocity(float velocity);

    void setOnHitGroundCallback(std::function <void(float)> callback);

private:
    void updateFriction();
    void updateDampingAndGravity();
    void updateOnGround(PhysicsManager &physicsManager);
    void updateHitGround();
    float getMoveSpeedMultiplier(PhysicsManager &physicsManager) const;
    float getHeightDiffBetweenNextStep(PhysicsManager &physicsManager) const;

    static const float k_customGravity;
    static const float k_defaultJumpVelocity;
    static const float k_frictionWhileStanding;
    static const float k_frictionWhileMoving;
    static const float k_rayEpsilon;
    static const float k_rayHeightToDetermineIfStandingOnGround;
    static const float k_distanceBetweenStepsForRays;
    static const float k_minSlopeToStartSlowingDown;
    static const float k_maxSlopeToMove;
    static const float k_firstRayHeightToDetermineHeightDiffBetweenNextStep;
    static const float k_secondRayHeightToDetermineHeightDiffBetweenNextStep;
    static const float k_minVelDiffToNotifyAboutHittingGround;
    static const float k_dampingUnderWater;
    static const float k_dampingWhenSwimmingButNotUnderWater;
    static const float k_dampingWhenFlying;
    static const float k_swimmingMoveSpeedMultiplier;

    const float m_heightToEyes;
    const float m_waterLevel;
    const bool m_canFly;

    std::shared_ptr <RigidBody> m_rigidBody;
    FloatVec3 m_movement;
    bool m_previouslyMoving;
    bool m_onGround;
    bool m_nowJumping;
    bool m_wasWalkingPreviousFrame;
    bool m_wasTryingToMovePreviousFrame;
    int m_bodyUserIndexOnWhichWalkedPreviousFrame;
    float m_previousFrameVelY;
    float m_jumpVelocity;

    std::function <void(float)> m_onHitGroundCallback;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_DYNAMIC_CHARACTER_CONTROLLER_HPP
