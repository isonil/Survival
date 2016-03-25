#include "DynamicCharacterController.hpp"

#include "RigidBody.hpp"
#include "../managers/PhysicsManager.hpp"

namespace engine
{
namespace app3D
{

DynamicCharacterController::DynamicCharacterController(const std::shared_ptr <RigidBody> &rigidBody, float heightToEyes, float waterLevel, bool canFly)
    : m_heightToEyes{heightToEyes},
      m_waterLevel{waterLevel},
      m_canFly{canFly},
      m_rigidBody{rigidBody},
      m_previouslyMoving{},
      m_onGround{},
      m_nowJumping{}, // true if jumped and going up (if character starts to fall down, then m_nowJumping is false)
      m_wasWalkingPreviousFrame{},
      m_wasTryingToMovePreviousFrame{},
      m_bodyUserIndexOnWhichWalkedPreviousFrame{-1},
      m_previousFrameVelY{},
      m_jumpVelocity{k_defaultJumpVelocity}
{
    if(!m_rigidBody)
        throw Exception{"Rigid body is nullptr."};

    m_rigidBody->lockFallingOver();
    m_rigidBody->disableDeactivationState();

    if(m_canFly)
        m_rigidBody->setGravity(0.f);
    else
        m_rigidBody->setGravity(k_customGravity);
}

void DynamicCharacterController::update(PhysicsManager &physicsManager)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    updateFriction();
    updateDampingAndGravity();
    updateOnGround(physicsManager);
    updateHitGround();

    m_wasTryingToMovePreviousFrame = !m_movement.isFuzzyZero();
    m_wasWalkingPreviousFrame = !m_canFly && m_onGround && !m_movement.isFuzzyZero() && !isSwimming();

    float moveSpeedMultiplier{getMoveSpeedMultiplier(physicsManager)};
    float currentVelY{m_rigidBody->getLinearVelocity().y};
    bool moveAsMovement{!m_movement.isFuzzyZero()};

    // if character stopped moving but he was moving the frame before,
    // and not under water or flying then we have to stop it immediately
    if(m_movement.isFuzzyZero() && m_previouslyMoving && !isSwimming() && !m_canFly) {
        // if going up and character didn't jump, then set y velocity to 0 too,
        // it prevents characters from bouncing right after they stop moving
        if(currentVelY > 0.f && !m_nowJumping)
            m_rigidBody->setLinearVelocity({0.f, 0.f, 0.f});
        else
            m_rigidBody->setLinearVelocity({0.f, currentVelY, 0.f});

        moveAsMovement = false;
    }

    if(moveAsMovement) {
        if(isSwimming() || m_canFly) {
            float velY{currentVelY};

            if(std::fabs(m_movement.y * moveSpeedMultiplier) > std::fabs(currentVelY))
                velY = m_movement.y * moveSpeedMultiplier;

            m_rigidBody->setLinearVelocity({m_movement.x * moveSpeedMultiplier,
                                            velY,
                                            m_movement.z * moveSpeedMultiplier});
        }
        else {
            m_rigidBody->setLinearVelocity({m_movement.x * moveSpeedMultiplier,
                                            currentVelY,
                                            m_movement.z * moveSpeedMultiplier});
        }

        m_previouslyMoving = true;
    }
    else
        m_previouslyMoving = false;

    currentVelY = m_rigidBody->getLinearVelocity().y;

    if(currentVelY < 0.f || Math::fuzzyCompare(currentVelY, 0.f))
        m_nowJumping = false;
}

bool DynamicCharacterController::wasWalkingPreviousFrame() const
{
    return m_wasWalkingPreviousFrame;
}

bool DynamicCharacterController::wasTryingToMovePreviousFrame() const
{
    return m_wasTryingToMovePreviousFrame;
}

int DynamicCharacterController::getBodyUserIndexOnWhichWalkedPreviousFrame() const
{
    if(!wasWalkingPreviousFrame())
        return -1;

    return m_bodyUserIndexOnWhichWalkedPreviousFrame;
}

bool DynamicCharacterController::isOnGround() const
{
    return m_onGround;
}

bool DynamicCharacterController::canJump() const
{
    return !m_canFly && m_onGround && !m_nowJumping && !isSwimming();
}

void DynamicCharacterController::jump()
{
    if(!canJump())
        return;

    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    const auto &currentVel = m_rigidBody->getLinearVelocity();
    m_rigidBody->setLinearVelocity({currentVel.x, m_jumpVelocity, currentVel.z});
    m_nowJumping = true;
}

void DynamicCharacterController::setMovement(const FloatVec3 &movement)
{
    m_movement = movement;
}

void DynamicCharacterController::setPosition(const FloatVec3 &pos)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    m_rigidBody->setPosition(pos);
    m_rigidBody->setLinearVelocity({0.f, 0.f, 0.f});
}

const FloatVec3 &DynamicCharacterController::getMovement() const
{
    return m_movement;
}

FloatVec3 DynamicCharacterController::getPosition() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return m_rigidBody->getPosition();
}

FloatVec3 DynamicCharacterController::getLinearVelocity() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return m_rigidBody->getLinearVelocity();
}

bool DynamicCharacterController::rayTest_notMe(PhysicsManager &physicsManager, const FloatVec3 &start, const FloatVec3 &end, CollisionFilter withWhatCollide, FloatVec3 &outPos, int &outHitBodyUserIndex)
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return physicsManager.rayTest_notMe(start, end, m_rigidBody, withWhatCollide, outPos, outHitBodyUserIndex);
}

bool DynamicCharacterController::isSwimming() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return m_rigidBody->getPosition().y + m_heightToEyes * 0.6f < m_waterLevel;
}

bool DynamicCharacterController::isUnderWater() const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    return m_rigidBody->getPosition().y + m_heightToEyes < m_waterLevel;
}

void DynamicCharacterController::setJumpVelocity(float velocity)
{
    if(velocity < 0.f)
        velocity = 0.f;

    m_jumpVelocity = velocity;
}

void DynamicCharacterController::setOnHitGroundCallback(std::function <void(float)> callback)
{
    m_onHitGroundCallback = callback;
}

void DynamicCharacterController::updateFriction()
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    if(m_movement.isFuzzyZero())
        m_rigidBody->setFriction(k_frictionWhileStanding, k_frictionWhileStanding);
    else
        m_rigidBody->setFriction(k_frictionWhileMoving, k_frictionWhileMoving);
}

void DynamicCharacterController::updateDampingAndGravity()
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    if(isSwimming()) {
        m_rigidBody->setGravity(0.f);

        if(isUnderWater())
            m_rigidBody->setDamping(k_dampingUnderWater);
        else
            m_rigidBody->setDamping(k_dampingWhenSwimmingButNotUnderWater);
    }
    else if(m_canFly) {
        m_rigidBody->setDamping(k_dampingWhenFlying);
        m_rigidBody->setGravity(0.f);
    }
    else {
        m_rigidBody->setDamping(0.f);
        m_rigidBody->setGravity(k_customGravity);
    }
}

void DynamicCharacterController::updateOnGround(PhysicsManager &physicsManager)
{
    if(m_canFly || isSwimming()) {
        m_onGround = false;
        return;
    }

    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    const auto &rayFrom = m_rigidBody->getPosition();

    FloatVec3 rayPos;
    m_bodyUserIndexOnWhichWalkedPreviousFrame = -1;

    // we move ray slightly up, because usually character position is exactly as ground level
    bool ray{physicsManager.rayTest_notMe(rayFrom.movedY(k_rayEpsilon), rayFrom.movedY(-k_rayHeightToDetermineIfStandingOnGround), m_rigidBody, CollisionFilter::AllReal, rayPos, m_bodyUserIndexOnWhichWalkedPreviousFrame)};

    m_onGround = ray;
}

void DynamicCharacterController::updateHitGround()
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    float velY{m_rigidBody->getLinearVelocity().y};

    if(m_onGround && m_previousFrameVelY < 0.f && m_previousFrameVelY < velY && m_onHitGroundCallback) {
        float diff{std::fabs(velY - m_previousFrameVelY)};

        if(diff > k_minVelDiffToNotifyAboutHittingGround)
            m_onHitGroundCallback(diff);
    }

    m_previousFrameVelY = velY;
}

float DynamicCharacterController::getMoveSpeedMultiplier(PhysicsManager &physicsManager) const
{
    if(isSwimming())
        return k_swimmingMoveSpeedMultiplier;

    if(m_canFly)
        return 1.f;

    float heightDiff{getHeightDiffBetweenNextStep(physicsManager)};

    if(heightDiff > 0.f) {
        auto slope = static_cast <float> (atan(heightDiff / k_distanceBetweenStepsForRays));
        float multiplier{1.f};

        if(slope > k_minSlopeToStartSlowingDown)
            multiplier = 1.f - (slope - k_minSlopeToStartSlowingDown) / (k_maxSlopeToMove - k_minSlopeToStartSlowingDown);

        if(multiplier < 0.f)
            multiplier = 0.f;

        return multiplier;
    }

    return 1.f;
}

float DynamicCharacterController::getHeightDiffBetweenNextStep(PhysicsManager &physicsManager) const
{
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    if(m_movement.isFuzzyZero())
        return 0.f;

    if(m_canFly)
        return 0.f;

    const auto &movementDir = m_movement.normalized();

    const auto &firstRayFrom = m_rigidBody->getPosition();
    const auto &secondRayFrom = (firstRayFrom + movementDir * k_distanceBetweenStepsForRays);

    FloatVec3 rayPos1, rayPos2;
    int hitBodyUserIndex{-1};

    bool ray1{physicsManager.rayTest_notMe(firstRayFrom.movedY(k_rayEpsilon), firstRayFrom.movedY(-k_firstRayHeightToDetermineHeightDiffBetweenNextStep), m_rigidBody, CollisionFilter::AllReal, rayPos1, hitBodyUserIndex)};

    if(hitBodyUserIndex >= 0) // TODO HACK: temporary solution to movement problem (player sometimes becomes stuck)
        ray1 = false;

    bool ray2{};

    if(ray1) {
        ray2 = physicsManager.rayTest_notMe(secondRayFrom.movedY(k_rayEpsilon), secondRayFrom.movedY(-k_secondRayHeightToDetermineHeightDiffBetweenNextStep * 0.5f), m_rigidBody, CollisionFilter::AllReal, rayPos2, hitBodyUserIndex);

        if(!ray2) {
            const auto &secondRaySecondAttemptFrom = secondRayFrom.movedY(k_secondRayHeightToDetermineHeightDiffBetweenNextStep * 0.5f);
            ray2 = physicsManager.rayTest_notMe(secondRaySecondAttemptFrom, secondRaySecondAttemptFrom.movedY(-k_secondRayHeightToDetermineHeightDiffBetweenNextStep), m_rigidBody, CollisionFilter::AllReal, rayPos2, hitBodyUserIndex);
        }

        if(hitBodyUserIndex >= 0) // TODO HACK: temporary solution to movement problem (player sometimes becomes stuck)
            ray2 = false;
    }

    if(ray1 && ray2)
        return rayPos2.y - rayPos1.y;

    return 0.f;
}

const float DynamicCharacterController::k_customGravity{PhysicsManager::k_gravity * 2.f};
const float DynamicCharacterController::k_defaultJumpVelocity{8.5f};
const float DynamicCharacterController::k_frictionWhileStanding{1.f};
const float DynamicCharacterController::k_frictionWhileMoving{0.5f};
const float DynamicCharacterController::k_rayEpsilon{0.3f};
const float DynamicCharacterController::k_rayHeightToDetermineIfStandingOnGround{1.f};
const float DynamicCharacterController::k_distanceBetweenStepsForRays{0.5f};
const float DynamicCharacterController::k_minSlopeToStartSlowingDown{Math::degToRad(30.f)};
const float DynamicCharacterController::k_maxSlopeToMove{Math::degToRad(50.f)};
const float DynamicCharacterController::k_firstRayHeightToDetermineHeightDiffBetweenNextStep{3.f};
const float DynamicCharacterController::k_secondRayHeightToDetermineHeightDiffBetweenNextStep{100.f};
const float DynamicCharacterController::k_minVelDiffToNotifyAboutHittingGround{3.f};
const float DynamicCharacterController::k_dampingUnderWater{0.8f};
const float DynamicCharacterController::k_dampingWhenSwimmingButNotUnderWater{0.9f};
const float DynamicCharacterController::k_dampingWhenFlying{0.92f};
const float DynamicCharacterController::k_swimmingMoveSpeedMultiplier{0.5f};

} // namespace app3D
} // namespace engine
