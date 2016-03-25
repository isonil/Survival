#include "NPCComponent.hpp"

#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../world/World.hpp"
#include "../../world/WorldPart.hpp"
#include "../../defs/ItemDef.hpp"
#include "../../defs/FactionDef.hpp"
#include "../../itemContainers/SingleSlotItemContainer.hpp"
#include "../../world/WorldPartFreePosFinder.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/app3D/Device.hpp"
#include "../Character.hpp"
#include "../Item.hpp"

namespace app
{

NPCComponent::NPCComponent(Character &character)
    : CharacterComponent{character},
      m_target{Target::None},
      m_attackedEntityPreviously{},
      m_preferredRangeAttackDist{k_preferredRangeAttackDistRange.randomElement()}
{
}

void NPCComponent::onInWorldUpdate()
{
    TRACK;

    if(shouldNotDoAnything()) {
        getCharacter().setMovement({});
        return;
    }

    updateTarget();

    auto localGuard = m_targetEntity.lock(); // lifetime guard
    // from now on, if m_target is Entity, then there is a guarantee that target entity is valid

    // we can either attack, or move.
    // first, we try to attack
    bool attacked{updateAIAttack()};

    if(!attacked)
        updateAIMove();
}

void NPCComponent::onChangedTargetToObstacle()
{
}

bool NPCComponent::shouldRotateToFaceAttackDir() const
{
    return true;
}

bool NPCComponent::canTryToAttackTargetNow() const
{
    return true;
}

void NPCComponent::setTarget(Target target, const std::weak_ptr <Entity> &targetEntity, const engine::FloatVec2 &targetPosition)
{
    m_target = target;

    if(m_target == Target::Entity) {
        m_targetEntity = targetEntity;

        if(m_targetEntity.expired()) {
            m_target = Target::None;
            throw engine::Exception{"Tried to set Target::Entity with nullptr Entity."};
        }
    }
    else if(m_target == Target::Position)
        m_targetPosition = targetPosition;
    else
        m_targetEntity.reset();
}

NPCComponent::Target NPCComponent::getTarget() const
{
    return m_target;
}

Entity &NPCComponent::getTargetEntity() const
{
    if(m_target != Target::Entity)
        throw engine::Exception{"Tried to get target entity but m_target != Target::Entity."};

    const auto &shared = m_targetEntity.lock();

    if(!shared)
        throw engine::Exception{"Tried to get target entity (m_target == Target::Entity), but it is nullptr for some reason. It must have been destroyed mid-frame, but this should never happen."};

    return *shared;
}

const engine::FloatVec2 &NPCComponent::getTargetPosition() const
{
    if(m_target != Target::Position)
        throw engine::Exception{"Tried to get target position but m_target != Target::Position."};

    return m_targetPosition;
}

bool NPCComponent::targetEntityExpired() const
{
    return m_targetEntity.expired();
}

bool NPCComponent::isGoodTarget(const Entity &entity) const
{
    const auto &relation = getCharacter().getFactionDef().getRelation(entity.getFactionDef());

    return relation == FactionRelationDef::Relation::Hostile &&
           !entity.isKilled() &&
           entity.isInWorld();
}

bool NPCComponent::wantsToHit(int entityID) const
{
    if(entityID < 0)
        return false;

    auto &world = Global::getCore().getWorld();

    if(!world.entityExists(entityID))
        return false;

    return isGoodTarget(world.getEntity(entityID));
}

bool NPCComponent::usesMelee() const
{
    const auto &handsContainer = getCharacter().getInventory().getHandsItemContainer();

    return handsContainer.hasItem() && handsContainer.getItem().getDef().getOnUsed().isMelee();
}

void NPCComponent::updateTarget()
{
    TRACK;

    // note that when calculating new target, the chosen target will be in
    // most cases the same as previous one (if it's still the best one;
    // calculating new target should be deterministic)

    if(m_target == Target::None) {
        // calculate new target from time to time when entity doesn't have any

        if(m_calculateNewTargetWhenHavingNoTargetTimer.passed()) {
            m_calculateNewTargetWhenHavingNoTargetTimer.set(k_calculateNewTargetWhenHavingNoTargetTime + engine::Random::rangeInclusive(0.f, k_calculateNewTargetRandomTimeOffset));
            calculateNewTarget();
        }
    }
    else if(m_target == Target::Entity) {
        if(m_targetEntity.expired() || !isGoodTarget(*m_targetEntity.lock()) || m_calculateNewTargetWhenHavingEntityAsTargetTimer.passed()) {
            m_calculateNewTargetWhenHavingEntityAsTargetTimer.set(k_calculateNewTargetWhenHavingEntityAsTargetTime + engine::Random::rangeInclusive(0.f, k_calculateNewTargetRandomTimeOffset));
            calculateNewTarget();
        }
    }
    else if(m_target == Target::Position) {
        if(m_calculateNewTargetWhenHavingPositionAsTargetTimer.passed()) {
            m_calculateNewTargetWhenHavingPositionAsTargetTimer.set(k_calculateNewTargetWhenHavingPositionAsTargetTime + engine::Random::rangeInclusive(0.f, k_calculateNewTargetRandomTimeOffset));
            calculateNewTarget();
        }
    }

    // try to scan for obstacle in way (and possibly change target) from time to time

    if(m_scanForObstacleInWayTimer.passed()) {
        m_scanForObstacleInWayTimer.set(k_scanForObstacleInWayTime + engine::Random::rangeInclusive(0.f, k_scanForObstacleInWayRandomTimeOffset));
        scanForObstacleInWay();
    }
}

bool NPCComponent::updateAIAttack()
{
    // now we'll try to attack target entity

    if(m_target == Target::Entity && !m_targetEntity.expired()) {
        if(m_checkIfCanHitEntityTimer.passed() || m_attackedEntityPreviously) {
            m_checkIfCanHitEntityTimer.set(k_checkIfCanHitEntityTime + engine::Random::rangeInclusive(0.f, k_checkIfCanHitEntityRandomTimeOffset));

            bool attacked{};

            if(canTryToAttackTargetNow()) {
                if(usesMelee())
                    attacked = updateAIAttack_Entity_Melee();
                else
                    attacked = updateAIAttack_Entity_Range();
            }

            m_attackedEntityPreviously = attacked;

            if(attacked)
                return true;
        }
        else
            m_attackedEntityPreviously = false;
    }

    return false;
}

bool NPCComponent::updateAIAttack_Entity_Melee()
{
    TRACK;

    // here, we will try to attack our target using melee weapon

    const auto &targetEntityShared = m_targetEntity.lock();
    E_DASSERT(targetEntityShared, "Entity is nullptr.");

    auto &character = getCharacter();
    const auto &itemUseSourcePos = character.getNextItemUseSourcePos();
    const auto &targetAimPos = targetEntityShared->getAIAimPosition();
    const auto &dir = (targetAimPos - itemUseSourcePos).normalized();

    engine::FloatVec3 hitPos;
    int hitEntityID{-1};

    bool didHit{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + dir * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

    if(didHit && wantsToHit(hitEntityID)) {
        character.tryUseItemInHands(dir, shouldRotateToFaceAttackDir());
        return true;
    }

    return false;
}

bool NPCComponent::updateAIAttack_Entity_Range()
{
    TRACK;

    // here, we will try to attack our target using non-melee weapon

    const auto targetEntityShared = m_targetEntity.lock();
    E_DASSERT(targetEntityShared, "Entity is nullptr.");

    auto &character = getCharacter();
    const auto &itemUseSourcePos = character.getNextItemUseSourcePos();
    const auto &targetEntityPos = targetEntityShared->getInWorldPosition();
    const auto &targetAimPos = targetEntityShared->getAIAimPosition();
    const auto &dir = (targetAimPos - itemUseSourcePos).normalized();
    const auto &world = Global::getCore().getWorld();

    float heightAtTargetPos{world.getHeight(targetEntityPos)};
    float targetHeightDiffFromGround{std::fabs(targetEntityPos.y - heightAtTargetPos)};

    bool attack{};

    if(targetEntityPos.y > WorldPart::k_waterHeight && // target is above water level
       targetEntityPos.y > heightAtTargetPos + m_preferredRangeAttackDist && // target is higher than ground level + preferred range attack dist (so it will never be possible to attack it from preferred dist)
       itemUseSourcePos.getDistanceSq(targetEntityPos) < 2.f * targetHeightDiffFromGround * targetHeightDiffFromGround) { // I am close enough to the target (dist is less than X, see asciiart below)

        /*  /\
         * y|    T (target)
         *  |    | \
         *  |   h|  \X
         *  |    |   \
         *  |     ----M (me)
         *  |      h (the same as height)
         */

        // try very long range attack
        attack = true;
    }
    else if(itemUseSourcePos.getDistanceSq(targetAimPos) < m_preferredRangeAttackDist * m_preferredRangeAttackDist) {
        // try normal range attack at preferred dist
        attack = true;
    }

    if(attack) {
        engine::FloatVec3 hitPos;
        int hitEntityID{-1};

        bool didHit{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + dir * ItemDef::k_nonMeleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

        if(didHit && wantsToHit(hitEntityID)) {
            character.tryUseItemInHands(dir, shouldRotateToFaceAttackDir());
            return true;
        }
    }

    return false;
}

bool NPCComponent::shouldNotDoAnything() const
{
    if(!m_waitTimer.passed())
        return true;

    const auto &character = getCharacter();

    if(character.isBusy())
        return true;

    if(character.isKilled())
        return true;

    return false;
}

void NPCComponent::scanForObstacleInWay()
{
    TRACK;

    // note that we don't care if we use melee or range weapon here,
    // we scan at melee range anyway

    auto &character = getCharacter();
    // we scan using item use source position, because if we get
    // anything we need to be sure we will be able to hit it
    const auto &itemUseSourcePos = character.getNextItemUseSourcePos();

    engine::FloatVec3 hitPos;
    int hitEntityID{-1};

    // first check at horizontal dir

    const auto &horizontalDir = character.getHorizontalLookVec();
    bool didHitHorizontally{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + horizontalDir * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

    if(didHitHorizontally && wantsToHit(hitEntityID)) {
        setTarget(Target::Entity, Global::getCore().getWorld().getEntityPtr(hitEntityID));
        onChangedTargetToObstacle();
        return;
    }
    else {
        // now check diagonal dir

        const auto &diagonalDir1 = horizontalDir.changedY(-1.f).normalized();

        bool didHitDiagonally1{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + diagonalDir1 * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

        if(didHitDiagonally1 && wantsToHit(hitEntityID)) {
            setTarget(Target::Entity, Global::getCore().getWorld().getEntityPtr(hitEntityID));
            onChangedTargetToObstacle();
            return;
        }
        else {
            // check diagonal dir (more down)

            const auto &diagonalDir2 = horizontalDir.changedY(-1.f).normalized().changedY(-1.f).normalized();

            bool didHitDiagonally2{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + diagonalDir2 * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

            if(didHitDiagonally2 && wantsToHit(hitEntityID)) {
                setTarget(Target::Entity, Global::getCore().getWorld().getEntityPtr(hitEntityID));
                onChangedTargetToObstacle();
                return;
            }
            else {
                // check diagonal dir (even more down)

                const auto &diagonalDir3 = horizontalDir.changedY(-1.f).normalized().changedY(-1.f).normalized().changedY(-1.f).normalized();

                bool didHitDiagonally3{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + diagonalDir3 * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

                if(didHitDiagonally3 && wantsToHit(hitEntityID)) {
                    setTarget(Target::Entity, Global::getCore().getWorld().getEntityPtr(hitEntityID));
                    onChangedTargetToObstacle();
                    return;
                }
                else {
                    // check vertical dir (down)

                    engine::FloatVec3 verticalDirDown{0.f, -1.f, 0.f};
                    bool didHitVerticallyDown{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + verticalDirDown * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

                    if(didHitVerticallyDown && wantsToHit(hitEntityID)) {
                        setTarget(Target::Entity, Global::getCore().getWorld().getEntityPtr(hitEntityID));
                        onChangedTargetToObstacle();
                        return;
                    }
                    else {
                        // finally check vertical dir (up)

                        engine::FloatVec3 verticalDirUp{0.f, 1.f, 0.f};
                        bool didHitVerticallyUp{character.rayTest_notMe(itemUseSourcePos, itemUseSourcePos + verticalDirUp * ItemDef::k_meleeRange, engine::app3D::CollisionFilter::AllReal, hitPos, hitEntityID)};

                        if(didHitVerticallyUp && wantsToHit(hitEntityID)) {
                            setTarget(Target::Entity, Global::getCore().getWorld().getEntityPtr(hitEntityID));
                            onChangedTargetToObstacle();
                            return;
                        }
                    }
                }
            }
        }
    }
}

const float NPCComponent::k_calculateNewTargetWhenHavingNoTargetTime{1500.f};
const float NPCComponent::k_calculateNewTargetWhenHavingEntityAsTargetTime{2500.f};
const float NPCComponent::k_calculateNewTargetWhenHavingPositionAsTargetTime{2000.f};
const float NPCComponent::k_calculateNewTargetRandomTimeOffset{150.f};
const float NPCComponent::k_scanForObstacleInWayTime{100.f};
const float NPCComponent::k_scanForObstacleInWayRandomTimeOffset{100.f};
const float NPCComponent::k_checkIfCanHitEntityTime{100.f};
const float NPCComponent::k_checkIfCanHitEntityRandomTimeOffset{50.f};
const engine::FloatRange NPCComponent::k_preferredRangeAttackDistRange{30.f, 130.f};

} // namespace app
