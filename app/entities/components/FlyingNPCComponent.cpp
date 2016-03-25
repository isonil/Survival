#include "FlyingNPCComponent.hpp"

#include "../../world/World.hpp"
#include "../../world/WorldPart.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../character/CharacterStatsOrSkillsRelatedFormulas.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "../Character.hpp"

namespace app
{

FlyingNPCComponent::FlyingNPCComponent(Character &character)
    : NPCComponent{character},
      m_setNonZeroMovementThisFrame{},
      m_previousRayTestToTargetHitSomethingIWantToHit{},
      m_previousRayTestToTargetXZHitSomethingIDontWantToHit{},
      m_rotation{{}, InterpolationType::FixedStepLinear, k_rotationInterpolationStep, {0.f, 360.f}}
{
}

void FlyingNPCComponent::updateAIMove()
{
    m_setNonZeroMovementThisFrame = false;

    if(getTarget() == Target::Entity)
        updateAIMove_Entity();

    if(!m_setNonZeroMovementThisFrame)
        getCharacter().setMovement({});

    getCharacter().setInWorldRotation(m_rotation.getCurrentVec());
    m_rotation.update();
}

void FlyingNPCComponent::calculateNewTarget()
{
    TRACK;

    // calculating new target should be deterministic

    auto &world = Global::getCore().getWorld();
    auto &character = getCharacter();
    const auto &myPos = character.getInWorldPosition();

    int bestEntityID{-1};
    int bestPriority{-1};
    float bestDist{};

    world.forEachEntity([&bestPriority, &bestDist, &bestEntityID, &myPos, this](auto &entity) {
        int entityPriority{entity.getAIPotentialTargetPriority()};

        // has negative priority?
        if(entityPriority < 0)
            return;

        float dist{entity.getInWorldPosition().getDistanceSq(myPos)};

        // is out of max range?
        if(dist > k_maxTargetableEntityDistance * k_maxTargetableEntityDistance)
            return;

        // is it even a good target?
        if(!this->isGoodTarget(entity))
            return;

        bool makeBest{};

        if(entityPriority > bestPriority) // has higher priority than current best?
            makeBest = true;
        else if(entityPriority == bestPriority && dist < bestDist) // is closer than current best?
            makeBest = true;

        if(makeBest) {
            bestPriority = entityPriority;
            bestDist = dist;
            bestEntityID = entity.getEntityID();
        }
    });

    if(bestEntityID >= 0)
        setTarget(Target::Entity, world.getEntityPtr(bestEntityID));
    else if(getTarget() == Target::Entity)
        setTarget(Target::None);

    // TODO: add Position target (if there is no Entity target, then just wander around)
    // TODO: if changed Position target then set recalculate checkpoint timer to 0
}

bool FlyingNPCComponent::shouldRotateToFaceAttackDir() const
{
    return false;
}

bool FlyingNPCComponent::canTryToAttackTargetNow() const
{
    if(getTarget() != Target::Entity)
        return true;

    const auto &character = getCharacter();
    const auto &myPos = character.getInWorldPosition();
    const auto &targetPos = getTargetEntity().getAIAimPosition();

    const auto &irrDir1 = engine::app3D::IrrlichtConversions::toVector(character.getInWorldRotation()).rotationToDirection();
    const auto &irrDir2 = engine::app3D::IrrlichtConversions::toVector((targetPos - myPos).normalized());

    return irrDir1.dotProduct(irrDir2) > 0.97f; // arbitrary epsilon
}

void FlyingNPCComponent::updateAIMove_Entity()
{
    enum class Movement
    {
        None,
        TowardsTargetXZAndUp,
        TowardsTargetXZAndDown,
        TowardsTargetXZ,
        TowardsTarget
    };

    auto &character = getCharacter();
    const auto &targetPos = getTargetEntity().getAIAimPosition();
    const auto &myPos = character.getInWorldPosition();

    float distXZSq{engine::FloatVec2{targetPos.x, targetPos.z}.getDistanceSq(engine::FloatVec2{myPos.x, myPos.z})};
    auto movement = Movement::None;

    if(distXZSq > k_minDistToTryToGetDirectlyToTheTarget * k_minDistToTryToGetDirectlyToTheTarget) {
        if(myPos.y < k_minFlyHeightWhenFar)
            movement = Movement::TowardsTargetXZAndUp;
        else if(myPos.y > k_maxFlyHeightWhenFar)
            movement = Movement::TowardsTargetXZAndDown;
        else {
            updateRayTestToTargetXZ();

            if(m_previousRayTestToTargetXZHitSomethingIDontWantToHit)
                movement = Movement::TowardsTargetXZAndUp;
            else
                movement = Movement::TowardsTargetXZ;
        }
    }
    else {
        // trying to go directly to the target

        updateRayTestToTarget();

        if(m_previousRayTestToTargetHitSomethingIWantToHit)
            movement = Movement::TowardsTarget;
        else {
            if(distXZSq < 0.001f) // arbitrary epsilon value
                // if we are directly above target (or below), then just try to get to it anyway
                // (we won't be able to reach it, but it's still better than going up infinitely)
                movement = Movement::TowardsTarget;
            else
                movement = Movement::TowardsTargetXZAndUp;
        }
    }

    if(movement == Movement::None)
        return;

    engine::FloatVec3 movementDir;

    if(movement == Movement::TowardsTargetXZAndUp)
        movementDir = (targetPos - myPos).normalized().changedY(1.f).normalized();
    else if(movement == Movement::TowardsTargetXZAndDown)
        movementDir = (targetPos - myPos).normalized().changedY(-1.f).normalized();
    else if(movement == Movement::TowardsTargetXZ)
        movementDir = (targetPos - myPos).changedY(0.f).normalized();
    else if(movement == Movement::TowardsTarget)
        movementDir = (targetPos - myPos).normalized();

    const auto &irrRot = engine::app3D::IrrlichtConversions::toVector(movementDir).getHorizontalAngle();
    m_rotation.setTargetVec({irrRot.X, irrRot.Y, irrRot.Z});

    const auto &currentIrrMovementDir = engine::app3D::IrrlichtConversions::toVector(m_rotation.getCurrentVec()).rotationToDirection();
    engine::FloatVec3 currentMovementDir{currentIrrMovementDir.X, currentIrrMovementDir.Y, currentIrrMovementDir.Z};

    character.setMovement(currentMovementDir * CharacterStatsOrSkillsRelatedFormulas::getMoveSpeed(character));

    m_setNonZeroMovementThisFrame = true;
}

void FlyingNPCComponent::updateRayTestToTarget()
{
    const auto &character = getCharacter();
    const auto &itemUseSourcePos = getCharacter().getNextItemUseSourcePos();
    const auto &targetPos = getTargetEntity().getAIAimPosition();

    if(m_rayTestToTargetTimer.passed()) {
        m_rayTestToTargetTimer.set(k_rayTestToTargetTime + engine::Random::rangeInclusive(0.f, k_rayTestToTargetRandomTimeOffset));

        engine::FloatVec3 hitPos;
        int hitEntityID{-1};

        const auto &dir = (targetPos - itemUseSourcePos).normalized();

        bool didHit{character.rayTest_notMe(itemUseSourcePos, targetPos + dir * 1.2f, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

        if(didHit && wantsToHit(hitEntityID))
            m_previousRayTestToTargetHitSomethingIWantToHit = true;
        else
            m_previousRayTestToTargetHitSomethingIWantToHit = false;
    }
}

void FlyingNPCComponent::updateRayTestToTargetXZ()
{
    const auto &character = getCharacter();
    const auto &itemUseSourcePos = getCharacter().getNextItemUseSourcePos();
    const auto &targetPos = getTargetEntity().getAIAimPosition();

    if(m_rayTestToTargetXZTimer.passed()) {
        m_rayTestToTargetXZTimer.set(k_rayTestToTargetTime + engine::Random::rangeInclusive(0.f, k_rayTestToTargetRandomTimeOffset));

        engine::FloatVec3 hitPos;
        int hitEntityID{-1};

        const auto &dir = (targetPos - itemUseSourcePos).changedY(0.f).normalized();

        bool didHit{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + dir * k_rayTestToTargetXZRayLength, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

        if(didHit && !wantsToHit(hitEntityID))
            m_previousRayTestToTargetXZHitSomethingIDontWantToHit = true;
        else
            m_previousRayTestToTargetXZHitSomethingIDontWantToHit = false;
    }
}

const float FlyingNPCComponent::k_minDistToTryToGetDirectlyToTheTarget{150.f};
const float FlyingNPCComponent::k_minFlyHeightWhenFar{WorldPart::k_waterHeight + 2.f};
const float FlyingNPCComponent::k_maxFlyHeightWhenFar{250.f};
const float FlyingNPCComponent::k_rayTestToTargetTime{100.f};
const float FlyingNPCComponent::k_rayTestToTargetRandomTimeOffset{150.f};
const float FlyingNPCComponent::k_rayTestToTargetXZRayLength{5.f};
const float FlyingNPCComponent::k_maxTargetableEntityDistance{1200.f};
const float FlyingNPCComponent::k_rotationInterpolationStep{100.f};

} // namespace app
