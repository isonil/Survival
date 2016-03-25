#ifndef APP_TURRET_COMPONENT_HPP
#define APP_TURRET_COMPONENT_HPP

#include "../../util/Timer.hpp"
#include "../../util/InterpolatedLoopedFloatVec3.hpp"
#include "engine/util/Vec3.hpp"
#include "engine/app3D/Sound.hpp"

namespace app
{

class Structure;
class Entity;
class Item;

class TurretComponent
{
public:
    TurretComponent(Structure &structure);

    void onInWorldUpdate();

private:
    void updateDistancesToCollisions();
    void updateAI();
    void updateHeadMoveSound();
    void calculateNewTarget();
    void tryShoot(const engine::FloatVec3 &shootAt);
    void doIdleRotation();

    float getYAngle(const engine::FloatVec3 &pos1, const engine::FloatVec3 &pos2) const;
    bool isGoodTarget(const Entity &entity) const;

    static const float k_turretHeadRotDistanceToShoot;
    static const float k_turretHeadRotInterpolationStep_withTarget;
    static const float k_turretHeadRotInterpolationStep_noTarget;
    static const float k_maxYAngle;
    static const float k_maxDistanceToTarget;
    static const float k_maxScanDistanceForDistanceToCollision;
    static const float k_updateDistancesToCollisionsTime;
    static const float k_updateDistancesToCollisionsRandomTimeOffset;
    static const float k_calculateNewTargetTime;
    static const float k_calculateNewTargetRandomTimeOffset;
    static const float k_rotateWhileIdleTime;
    static const float k_rotateWhileIdleRandomTimeOffset;

    Structure &m_structure;

    engine::app3D::Sound m_headMoveSound;

    float m_frontDistanceToCollision;
    float m_backDistanceToCollision;
    float m_leftDistanceToCollision;
    float m_rightDistanceToCollision;

    Timer m_updateDistancesToCollisionsTimer;
    Timer m_calculateNewTargetTimer;
    Timer m_rotateWhileIdleTimer;
    Timer m_shootCooldownTimer;

    InterpolatedLoopedFloatVec3 m_turretHeadRot;

    bool m_hasTargetEntity;
    std::weak_ptr <Entity> m_targetEntity;

    std::shared_ptr <Item> m_turretWeapon;
};

} // namespace app

#endif // APP_TURRET_COMPONENT_HPP
