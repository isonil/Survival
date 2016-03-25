#include "MovingOnGroundNPCComponent.hpp"

#include "../../world/World.hpp"
#include "../../world/WorldPart.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../character/CharacterStatsOrSkillsRelatedFormulas.hpp"
#include "../Character.hpp"

namespace app
{

MovingOnGroundNPCComponent::MovingOnGroundNPCComponent(Character &character)
    : NPCComponent{character},
      m_setNonZeroMovementThisFrame{}
{
}

void MovingOnGroundNPCComponent::updateAIMove()
{
    TRACK;

    m_setNonZeroMovementThisFrame = false;

    // update actual Target-related AI

    if(getTarget() == Target::Entity)
        updateAIMove_Entity();
    else if(getTarget() == Target::Position)
        updateAIMove_Position();

    if(!m_setNonZeroMovementThisFrame)
        getCharacter().setMovement({});
}

void MovingOnGroundNPCComponent::calculateNewTarget()
{
    TRACK;

    // calculating new target should be deterministic

    auto &world = Global::getCore().getWorld();
    auto &character = getCharacter();
    const auto &myPos = character.getInWorldPosition();

    int previousTargetEntityID{-1};

    if(getTarget() == Target::Entity && !targetEntityExpired())
        previousTargetEntityID = getTargetEntity().getEntityID();

    int bestEntityID{-1};

    if(usesMelee()) {
        bool bestReachable{};
        int bestPriority{-1};
        float bestDist{};

        world.forEachEntity([&bestReachable, &bestPriority, &bestDist, &bestEntityID, &myPos, &character, this](auto &entity) {
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

            bool entityReachable{entity.isPotentiallyMeleeReachableBy(character)};
            bool makeBest{};

            if(!bestReachable && entityReachable) // 'reachable' is the most important factor
                makeBest = true;
            else if(bestReachable == entityReachable && entityPriority > bestPriority) // has higher priority than current best?
                makeBest = true;
            else if(entityPriority == bestPriority && dist < bestDist) // is closer than current best?
                makeBest = true;

            if(makeBest) {
                bestReachable = entityReachable;
                bestPriority = entityPriority;
                bestDist = dist;
                bestEntityID = entity.getEntityID();
            }
        });
    }
    else {
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
    }

    if(bestEntityID >= 0)
        setTarget(Target::Entity, world.getEntityPtr(bestEntityID));
    else if(getTarget() == Target::Entity)
        setTarget(Target::None);

    // TODO: add Position target (if there is no Entity target, then just wander around)
    // TODO: if changed Position target then set recalculate checkpoint timer to 0

    int currentTargetEntityID{-1};

    if(getTarget() == Target::Entity && !targetEntityExpired())
        currentTargetEntityID = getTargetEntity().getEntityID();

    if(previousTargetEntityID != currentTargetEntityID)
        m_recalculateMoveCheckpointTimer.set(0.0); // force recalculate checkpoint if got new target
}

void MovingOnGroundNPCComponent::onChangedTargetToObstacle()
{
    m_recalculateMoveCheckpointTimer.set(0.0);
}

void MovingOnGroundNPCComponent::updateAIMove_Entity()
{
    TRACK;

    const auto &targetEntity = getTargetEntity();

    // calculate new checkpoint if reached it or some time passed

    if(m_recalculateMoveCheckpointTimer.passed() || didReachCurrentMoveCheckpoint()) {
        // if the target is far, then calculate new checkpoint less often

        const auto &pos = getCharacter().getInWorldPosition();
        const auto &targetPos = targetEntity.getInWorldPosition();

        float distRelatedTimeOffset{engine::Math::clamp01(pos.getDistance(targetPos) / k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxDist) * k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxTime};

        m_recalculateMoveCheckpointTimer.set(k_recalculateMoveCheckpointTime + engine::Random::rangeInclusive(0.f, k_recalculateMoveCheckpointRandomTimeOffset) + distRelatedTimeOffset);

        auto &world = Global::getCore().getWorld();
        auto *worldPart = world.getWorldPart(pos);

        if(worldPart) {
            const auto &checkpoint = worldPart->getPathFoundNextCheckpoint_worldPos({pos.x, pos.z}, {targetPos.x, targetPos.z}).first;
            m_currentMoveCheckpoint = {checkpoint.x, checkpoint.z};
        }
        else
            m_currentMoveCheckpoint = {targetPos.x, targetPos.z};
    }

    makeMoveTowardsCheckpoint();
}

void MovingOnGroundNPCComponent::updateAIMove_Position()
{
    TRACK;

    // calculate new checkpoint if reached it or some time passed

    if(m_recalculateMoveCheckpointTimer.passed() || didReachCurrentMoveCheckpoint()) {
        // if the target is far, then calculate new checkpoint less often

        const auto &pos = getCharacter().getInWorldPosition();
        const auto &targetPosition = getTargetPosition();

        float distRelatedTimeOffset{engine::Math::clamp01(targetPosition.getDistance({pos.x, pos.z}) / k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxDist) * k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxTime};

        m_recalculateMoveCheckpointTimer.set(k_recalculateMoveCheckpointTime + engine::Random::rangeInclusive(0.f, k_recalculateMoveCheckpointRandomTimeOffset) + distRelatedTimeOffset);

        auto &world = Global::getCore().getWorld();
        auto *worldPart = world.getWorldPart(pos);

        // we carefully check if we can reach the target position, otherwise we want to recalculate it later,
        // this is because position target is used for wandering around and reaching target is not important

        if(worldPart) {
            engine::FloatVec3 checkpoint;
            bool clearWay{};

            std::tie(checkpoint, clearWay) = worldPart->getPathFoundNextCheckpoint_worldPos({pos.x, pos.z}, targetPosition);

            if(clearWay)
                m_currentMoveCheckpoint = {checkpoint.x, checkpoint.z};
            else
                setTarget(Target::None);
        }
        else
            setTarget(Target::None);
    }

    if(getTarget() == Target::None)
        return;

    makeMoveTowardsCheckpoint();
}

void MovingOnGroundNPCComponent::makeMoveTowardsCheckpoint()
{
    TRACK;

    // set rotation and movement

    auto &character = getCharacter();
    const auto &pos = character.getInWorldPosition();

    character.setInWorldRotationToFace(m_currentMoveCheckpoint);

    engine::FloatVec2 pos2d{pos.x, pos.z};

    const auto &movement = (m_currentMoveCheckpoint - pos2d).normalized() * CharacterStatsOrSkillsRelatedFormulas::getMoveSpeed(character);

    character.setMovement2D(movement);

    m_setNonZeroMovementThisFrame = true;
}

bool MovingOnGroundNPCComponent::didReachCurrentMoveCheckpoint() const
{
    const auto &pos = getCharacter().getInWorldPosition();

    float diffX{std::fabs(pos.x - m_currentMoveCheckpoint.x)};
    float diffZ{std::fabs(pos.z - m_currentMoveCheckpoint.y)}; // 2d vs 3d

    return diffX < 0.1f && diffZ < 0.1f; // arbitrary epsilon values
}

const float MovingOnGroundNPCComponent::k_recalculateMoveCheckpointTime{220.f};
const float MovingOnGroundNPCComponent::k_recalculateMoveCheckpointRandomTimeOffset{100.f};
const float MovingOnGroundNPCComponent::k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxDist{1000.f};
const float MovingOnGroundNPCComponent::k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxTime{3000.f};
const float MovingOnGroundNPCComponent::k_maxTargetableEntityDistance{1200.f};

} // namespace app
