#include "TurretComponent.hpp"

#include "../../defs/DefsCache.hpp"
#include "../../defs/StructureDef.hpp"
#include "../../defs/ItemDef.hpp"
#include "../../defs/FactionDef.hpp"
#include "../../world/World.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../../SoundPool.hpp"
#include "../../EffectsPool.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "engine/util/Util.hpp"
#include "../Entity.hpp"
#include "../Structure.hpp"
#include "../Item.hpp"
#include "ElectricityComponent.hpp"

namespace app
{

TurretComponent::TurretComponent(Structure &structure)
    : m_structure{structure},
      m_headMoveSound{Global::getCore().getDefsCache().Sound_TurretHeadMove},
      m_frontDistanceToCollision{},
      m_backDistanceToCollision{},
      m_leftDistanceToCollision{},
      m_rightDistanceToCollision{},
      m_turretHeadRot{{}, InterpolationType::FixedStepLinear, k_turretHeadRotInterpolationStep_noTarget, {0.f, 360.f}},
      m_hasTargetEntity{}
{
    auto &world = Global::getCore().getWorld();
    const auto &itemDef = m_structure.getDef().getTurretInfo().getWeaponItemDefPtr();

    m_turretWeapon = std::make_shared <Item> (world.getUniqueEntityID(), itemDef);
}

void TurretComponent::onInWorldUpdate()
{
    updateDistancesToCollisions();
    updateAI();
    updateHeadMoveSound();

    m_turretHeadRot.update();
    m_structure.setTurretHeadRotation(m_turretHeadRot.getCurrentVec());

    if(m_targetEntity.expired())
        m_turretHeadRot.setNewStep(k_turretHeadRotInterpolationStep_noTarget);
    else
        m_turretHeadRot.setNewStep(k_turretHeadRotInterpolationStep_withTarget);
}

void TurretComponent::updateDistancesToCollisions()
{
    if(m_updateDistancesToCollisionsTimer.passed()) {
        m_updateDistancesToCollisionsTimer.set(k_updateDistancesToCollisionsTime + engine::Random::rangeInclusive(0.f, k_updateDistancesToCollisionsRandomTimeOffset));

        const auto &headPos = m_structure.getInWorldPosition().movedY(m_structure.getDef().getTurretInfo().getDistanceToHead());

        engine::FloatVec3 hitPos;
        int unused{-1};

        if(m_structure.rayTest_notTurretHead(headPos, headPos.movedX(-k_maxScanDistanceForDistanceToCollision), engine::app3D::CollisionFilter::AllReal, hitPos, unused))
            m_leftDistanceToCollision = std::fabs(headPos.x - hitPos.x);
        else
            m_leftDistanceToCollision = k_maxScanDistanceForDistanceToCollision;

        if(m_structure.rayTest_notTurretHead(headPos, headPos.movedX(k_maxScanDistanceForDistanceToCollision), engine::app3D::CollisionFilter::AllReal, hitPos, unused))
            m_rightDistanceToCollision = std::fabs(headPos.x - hitPos.x);
        else
            m_rightDistanceToCollision = k_maxScanDistanceForDistanceToCollision;

        if(m_structure.rayTest_notTurretHead(headPos, headPos.movedZ(k_maxScanDistanceForDistanceToCollision), engine::app3D::CollisionFilter::AllReal, hitPos, unused))
            m_frontDistanceToCollision = std::fabs(headPos.z - hitPos.z);
        else
            m_frontDistanceToCollision = k_maxScanDistanceForDistanceToCollision;

        if(m_structure.rayTest_notTurretHead(headPos, headPos.movedZ(-k_maxScanDistanceForDistanceToCollision), engine::app3D::CollisionFilter::AllReal, hitPos, unused))
            m_backDistanceToCollision = std::fabs(headPos.z - hitPos.z);
        else
            m_backDistanceToCollision = k_maxScanDistanceForDistanceToCollision;
    }
}

void TurretComponent::updateAI()
{
    if(m_structure.getDef().usesElectricity() && !m_structure.getElectricityComponent().isWorking()) {
        m_turretHeadRot.setVecWithoutInterpolation(m_turretHeadRot.getCurrentVec());
        m_targetEntity.reset();
        m_hasTargetEntity = false;

        return;
    }

    const auto &targetEntityShared = m_targetEntity.lock();

    if((m_hasTargetEntity && !targetEntityShared) || // we had a target but it disappeared somehow
       (m_hasTargetEntity && targetEntityShared && !isGoodTarget(*targetEntityShared)) || // we have a target but it's no longer good
       m_calculateNewTargetTimer.passed()) {
        m_calculateNewTargetTimer.set(k_calculateNewTargetTime + engine::Random::rangeInclusive(0.f, k_calculateNewTargetRandomTimeOffset));
        calculateNewTarget();
    }

    if(targetEntityShared) {
        const auto &pos = m_structure.getInWorldPosition();
        const auto &targetPos = targetEntityShared->getAIAimPosition();
        const auto &dir = (targetPos - pos).normalized();
        const auto &irrDir = engine::app3D::IrrlichtConversions::toVector(dir);
        const auto &horizontalAngle = irrDir.getHorizontalAngle();

        m_turretHeadRot.setTargetVec({horizontalAngle.X, horizontalAngle.Y, 0.f});

        if(m_shootCooldownTimer.passed() &&
           m_structure.getTurretHeadRotation().getLoopedDistanceSq(m_turretHeadRot.getTargetVec(), {0.f, 360.f}) <= k_turretHeadRotDistanceToShoot * k_turretHeadRotDistanceToShoot) {
            E_DASSERT(m_turretWeapon, "Item is nullptr.");

            m_shootCooldownTimer.set(m_turretWeapon->getDef().getOnUsed().getMinDuration());
            tryShoot(targetPos);
        }
    }
    else {
        if(m_rotateWhileIdleTimer.passed()) {
            m_rotateWhileIdleTimer.set(k_rotateWhileIdleTime + engine::Random::rangeInclusive(0.f, k_rotateWhileIdleRandomTimeOffset));
            doIdleRotation();
        }
    }
}

void TurretComponent::updateHeadMoveSound()
{
    if(m_structure.getTurretHeadRotation().getLoopedDistanceSq(m_turretHeadRot.getTargetVec(), {0.f, 360.f}) > 0.1f) {
        if(m_headMoveSound.stopped())
            m_headMoveSound.fadeInLooped();

        m_headMoveSound.setPosition(m_structure.getInWorldPosition());
    }
    else
        m_headMoveSound.fadeOut();

    m_headMoveSound.update(Global::getCore().getAppTime());
}

void TurretComponent::calculateNewTarget()
{
    TRACK;

    // calculating new target should be deterministic

    auto &core = Global::getCore();
    auto &world = core.getWorld();
    const auto &myPos = m_structure.getInWorldPosition();
    const auto &turretHeadPos = myPos.movedY(m_structure.getDef().getTurretInfo().getDistanceToHead());

    int previousTargetEntityID{-1};

    if(!m_targetEntity.expired())
        previousTargetEntityID = m_targetEntity.lock()->getEntityID();

    int bestEntityID{-1};
    int bestPriority{-1};
    float bestDist{};

    world.forEachEntity([&bestPriority, &bestDist, &bestEntityID, &myPos, &turretHeadPos, this](auto &entity) {
        int entityPriority{entity.getAIPotentialTargetPriority()};

        // has negative priority?
        if(entityPriority < 0)
            return;

        float dist{entity.getInWorldPosition().getDistanceSq(myPos)};

        // is it a good target?
        if(!this->isGoodTarget(entity))
            return;

        bool makeBest{};

        if(entityPriority > bestPriority) // has higher priority than current best?
            makeBest = true;
        else if(entityPriority == bestPriority && dist < bestDist) // is closer than current best?
            makeBest = true;

        // now we'll check if it's reachable, (we kept it as a last check for better performance)
        if(makeBest) {
            const auto &entityAimPos = entity.getAIAimPosition();
            const auto &dir = (entityAimPos - turretHeadPos).normalized();

            int hitEntityID{-1};
            engine::FloatVec3 unused;

            if(this->m_structure.rayTest_notTurretHead(turretHeadPos, entityAimPos + dir * 1.5f, engine::app3D::CollisionFilter::AllReal, unused, hitEntityID)) {
                if(hitEntityID != entity.getEntityID())
                    makeBest = false;
            }
        }

        if(makeBest) {
            bestPriority = entityPriority;
            bestDist = dist;
            bestEntityID = entity.getEntityID();
        }
    });

    if(bestEntityID >= 0)
        m_targetEntity = world.getEntityPtr(bestEntityID);
    else
        m_targetEntity.reset();

    int currentTargetEntityID{-1};

    if(!m_targetEntity.expired())
        currentTargetEntityID = m_targetEntity.lock()->getEntityID();

    if(previousTargetEntityID != currentTargetEntityID) {
        m_rotateWhileIdleTimer.set(k_rotateWhileIdleTime + engine::Random::rangeInclusive(0.f, k_rotateWhileIdleRandomTimeOffset));

        if(currentTargetEntityID >= 0) {
            auto &defsCache = core.getDefsCache();
            auto &soundPool = core.getSoundPool();

            soundPool.play(defsCache.Sound_TargetAcquired, m_structure.getInWorldPosition());
        }
    }

    m_hasTargetEntity = currentTargetEntityID >= 0;
}

void TurretComponent::tryShoot(const engine::FloatVec3 &shootAt)
{
    TRACK;

    E_DASSERT(m_turretWeapon, "Item is nullptr.");

    const auto &def = m_structure.getDef();
    const auto &turretInfo = def.getTurretInfo();
    const auto &onUsed = m_turretWeapon->getDef().getOnUsed();
    const auto &rotatedBarrelOffset = turretInfo.getRandomBarrelOffsetRotated(m_structure.getTurretHeadRotation());
    const auto &rayStart = m_structure.getInWorldPosition().movedY(turretInfo.getDistanceToHead()) + rotatedBarrelOffset;
    const auto &direction = (shootAt - rayStart).normalized();
    engine::FloatVec3 rayEnd;

    float spread{onUsed.getMinProjectilesSpreadAngle()};

    const auto &directionWithOffset = engine::Util::getRandomPointOnSpherePart(direction, spread);

    if(onUsed.isMelee())
        rayEnd = rayStart + directionWithOffset * ItemDef::k_meleeRange;
    else
        rayEnd = rayStart + directionWithOffset * ItemDef::k_nonMeleeRange;

    engine::FloatVec3 rayHitPos;
    int rayHitEntityID{-1};
    bool didHit{m_structure.rayTest_notTurretHead(rayStart, rayEnd, engine::app3D::CollisionFilter::AllReal | engine::app3D::CollisionFilter::Water, rayHitPos, rayHitEntityID)};

    if(didHit) {
        // no friendly fire

        if(rayHitEntityID >= 0) {
            const auto &entity = Global::getCore().getWorld().getEntity(rayHitEntityID);

            if(entity.getFactionDef().getRelation(m_structure.getFactionDef()) == FactionRelationDef::Relation::Good)
                return;
        }

        m_turretWeapon->onUsedOnSomething(rayStart, rayHitPos, rayHitEntityID, m_structure);
    }

    auto &core = Global::getCore();

    if(onUsed.hasSoundDef())
        core.getSoundPool().play(onUsed.getSoundDefPtr(), rayStart);

    if(onUsed.hasEffectDef())
        core.getEffectsPool().add(onUsed.getEffectDefPtr(), rayStart, m_structure.getTurretHeadRotation());
}

void TurretComponent::doIdleRotation()
{
    float maxDist{std::max({m_frontDistanceToCollision, m_backDistanceToCollision, m_leftDistanceToCollision, m_rightDistanceToCollision})};

    std::vector <float> available;

    if(engine::Math::fuzzyCompare(m_frontDistanceToCollision, maxDist))
        available.push_back(0.f);

    if(engine::Math::fuzzyCompare(m_backDistanceToCollision, maxDist))
        available.push_back(180.f);

    if(engine::Math::fuzzyCompare(m_leftDistanceToCollision, maxDist))
        available.push_back(270.f);

    if(engine::Math::fuzzyCompare(m_rightDistanceToCollision, maxDist))
        available.push_back(90.f);

    if(available.empty())
        return;

    const auto &chosen = available[engine::Random::rangeExclusive(0, available.size())];

    float randomRot{chosen + engine::Random::rangeInclusive(-40.f, 40.f)};
    float yAngle{engine::Random::rangeInclusive(-15.f, 0.f)};

    irr::core::matrix4 mat1;
    mat1.setRotationDegrees({0.f, randomRot, 0.f});

    irr::core::matrix4 mat2;
    mat2.setRotationDegrees({yAngle, 0.f, 0.f});

    irr::core::vector3df vect{0.f, 0.f, 1.f};

    const auto &finMat = mat1 * mat2;
    finMat.rotateVect(vect);

    const auto &horizontalAngle = vect.getHorizontalAngle();

    m_turretHeadRot.setTargetVec({horizontalAngle.X, horizontalAngle.Y, 0.f});
}

float TurretComponent::getYAngle(const engine::FloatVec3 &pos1, const engine::FloatVec3 &pos2) const
{
    float distXZ = std::hypot(pos1.x - pos2.x, pos1.z - pos2.z);

    return engine::Math::radToDeg(std::atan2(pos2.y - pos1.y, distXZ));
}

bool TurretComponent::isGoodTarget(const Entity &entity) const
{
    const auto &relation = m_structure.getFactionDef().getRelation(entity.getFactionDef());

    const auto &structureInWorldPos = m_structure.getInWorldPosition();
    const auto &entityInWorldPos = entity.getInWorldPosition();

    return relation == FactionRelationDef::Relation::Hostile &&
           !entity.isKilled() &&
           entity.isInWorld() &&
           structureInWorldPos.getDistanceSq(entityInWorldPos) <= k_maxDistanceToTarget * k_maxDistanceToTarget &&
           std::fabs(getYAngle(structureInWorldPos, entity.getAIAimPosition())) <= k_maxYAngle;
}

const float TurretComponent::k_turretHeadRotDistanceToShoot{10.f};
const float TurretComponent::k_turretHeadRotInterpolationStep_withTarget{100.f};
const float TurretComponent::k_turretHeadRotInterpolationStep_noTarget{10.f};
const float TurretComponent::k_maxYAngle{50.f};
const float TurretComponent::k_maxDistanceToTarget{150.f};
const float TurretComponent::k_maxScanDistanceForDistanceToCollision{20.f};
const float TurretComponent::k_updateDistancesToCollisionsTime{30000.f};
const float TurretComponent::k_updateDistancesToCollisionsRandomTimeOffset{10000.f};
const float TurretComponent::k_calculateNewTargetTime{3000.f};
const float TurretComponent::k_calculateNewTargetRandomTimeOffset{2000.f};
const float TurretComponent::k_rotateWhileIdleTime{3000.f};
const float TurretComponent::k_rotateWhileIdleRandomTimeOffset{8000.f};

} // namespace app
