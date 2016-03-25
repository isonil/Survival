#ifndef APP_NPC_COMPONENT_HPP
#define APP_NPC_COMPONENT_HPP

#include "../../util/Timer.hpp"
#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"
#include "CharacterComponent.hpp"

#include <memory>

namespace app
{

class Entity;

class NPCComponent : public CharacterComponent
{
public:
    NPCComponent(Character &character);

    void onInWorldUpdate() override;

protected:
    enum class Target
    {
        None,
        Entity,
        Position
    };

    virtual void updateAIMove() = 0;
    virtual void calculateNewTarget() = 0;
    virtual void onChangedTargetToObstacle();
    virtual bool shouldRotateToFaceAttackDir() const;
    virtual bool canTryToAttackTargetNow() const;

    void setTarget(Target target, const std::weak_ptr <Entity> &targetEntity = {}, const engine::FloatVec2 &targetPosition = {});
    Target getTarget() const;

    Entity &getTargetEntity() const;
    const engine::FloatVec2 &getTargetPosition() const;
    bool targetEntityExpired() const;
    bool isGoodTarget(const Entity &entity) const;
    bool wantsToHit(int entityID) const;
    bool usesMelee() const;

private:
    void updateTarget();
    bool updateAIAttack();
    bool updateAIAttack_Entity_Melee();
    bool updateAIAttack_Entity_Range();

    bool shouldNotDoAnything() const;
    void scanForObstacleInWay();

    static const float k_calculateNewTargetWhenHavingNoTargetTime;
    static const float k_calculateNewTargetWhenHavingEntityAsTargetTime;
    static const float k_calculateNewTargetWhenHavingPositionAsTargetTime;
    static const float k_calculateNewTargetRandomTimeOffset;
    static const float k_scanForObstacleInWayTime;
    static const float k_scanForObstacleInWayRandomTimeOffset;
    static const float k_checkIfCanHitEntityTime;
    static const float k_checkIfCanHitEntityRandomTimeOffset;
    static const engine::FloatRange k_preferredRangeAttackDistRange;

    Target m_target;
    std::weak_ptr <Entity> m_targetEntity;
    engine::FloatVec2 m_targetPosition;
    Timer m_waitTimer;
    Timer m_calculateNewTargetWhenHavingNoTargetTimer;
    Timer m_calculateNewTargetWhenHavingEntityAsTargetTimer;
    Timer m_calculateNewTargetWhenHavingPositionAsTargetTimer;
    Timer m_scanForObstacleInWayTimer;
    Timer m_checkIfCanHitEntityTimer;
    bool m_attackedEntityPreviously;
    float m_preferredRangeAttackDist;
};

} // namespace app

#endif // APP_NPC_COMPONENT_HPP
