#ifndef APP_MOVING_ON_GROUND_NPC_COMPONENT_HPP
#define APP_MOVING_ON_GROUND_NPC_COMPONENT_HPP

#include "NPCComponent.hpp"

namespace app
{

class MovingOnGroundNPCComponent : public NPCComponent
{
public:
    MovingOnGroundNPCComponent(Character &character);

protected:
    void updateAIMove() override;
    void calculateNewTarget() override;
    void onChangedTargetToObstacle() override;

private:
    void updateAIMove_Entity();
    void updateAIMove_Position();

    void makeMoveTowardsCheckpoint();
    bool didReachCurrentMoveCheckpoint() const;

    static const float k_recalculateMoveCheckpointTime;
    static const float k_recalculateMoveCheckpointRandomTimeOffset;
    static const float k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxDist;
    static const float k_recalculateMoveCheckpointDistantRelatedTimeOffsetMaxTime;
    static const float k_maxTargetableEntityDistance;

    bool m_setNonZeroMovementThisFrame;
    engine::FloatVec2 m_currentMoveCheckpoint;
    Timer m_recalculateMoveCheckpointTimer;
};

} // namespace app

#endif // APP_MOVING_ON_GROUND_NPC_COMPONENT_HPP
