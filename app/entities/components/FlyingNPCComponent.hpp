#ifndef APP_FLYING_NPC_COMPONENT_HPP
#define APP_FLYING_NPC_COMPONENT_HPP

#include "../../util/InterpolatedLoopedFloatVec3.hpp"
#include "NPCComponent.hpp"

namespace app
{

class FlyingNPCComponent : public NPCComponent
{
public:
    FlyingNPCComponent(Character &character);

protected:
    void updateAIMove() override;
    void calculateNewTarget() override;
    bool shouldRotateToFaceAttackDir() const override;
    bool canTryToAttackTargetNow() const override;

private:
    void updateAIMove_Entity();
    void updateRayTestToTarget();
    void updateRayTestToTargetXZ();

    static const float k_minDistToTryToGetDirectlyToTheTarget;
    static const float k_minFlyHeightWhenFar;
    static const float k_maxFlyHeightWhenFar;
    static const float k_rayTestToTargetTime;
    static const float k_rayTestToTargetRandomTimeOffset;
    static const float k_rayTestToTargetXZRayLength;
    static const float k_maxTargetableEntityDistance;
    static const float k_rotationInterpolationStep;

    bool m_setNonZeroMovementThisFrame;
    bool m_previousRayTestToTargetHitSomethingIWantToHit;
    bool m_previousRayTestToTargetXZHitSomethingIDontWantToHit;
    Timer m_rayTestToTargetTimer;
    Timer m_rayTestToTargetXZTimer;
    InterpolatedLoopedFloatVec3 m_rotation;
};

} // namespace app

#endif // APP_FLYING_NPC_COMPONENT_HPP
