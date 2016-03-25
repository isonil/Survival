#ifndef APP_CHARACTER_HPP
#define APP_CHARACTER_HPP

#include "engine/app3D/physics/CollisionFilter.hpp"
#include "../util/Timer.hpp"
#include "engine/util/Trace.hpp"
#include "character/Inventory.hpp"
#include "character/CharacterStatsAccumulator.hpp"
#include "Entity.hpp"

#include <memory>

namespace engine { namespace app3D { class DynamicCharacterController; class Model; class RigidBody; class Armature; } }

namespace app
{

class CharacterDef;
class ProjectilesSpreadAngleManager;
class CharacterComponent;
class PlayerComponent;
class NPCComponent;

class Character : public Entity, public engine::Tracked <Character>
{
public:
    Character(int entityID, const std::shared_ptr <CharacterDef> &def, bool isPlayer);

    bool wantsEverInWorldUpdate() const override;
    void setInWorldPosition(const engine::FloatVec3 &pos) override;
    void setInWorldRotation(const engine::FloatVec3 &rot) override;
    bool wantsToBeRemovedFromWorld() const override;
    bool canBeRevived(const Character &doer) const override;
    bool isKilled() const override;
    engine::FloatVec3 getAIAimPosition() const override;
    int getAIPotentialTargetPriority() const override;
    bool isPotentiallyMeleeReachableBy(const Character &character) const override;
    bool tryPickUpItem(std::shared_ptr <Item> item) override;
    std::shared_ptr <EffectDef> getOnHitEffectDefPtr() const override;
    std::string getName() const override;
    void onInWorldUpdate() override;
    void onSpawnedInWorld() override;
    void onRemovedFromWorld() override;
    void onDraw2DInfoWhenPointed() override;
    void onItemUsedOnMe(Entity &doer, const Item &item) override;
    void onRevived(Character &doer) override;
    void onHitGround(float force) override;
    void onStoppedBusyAnimation() override;

    void tryUseItemInHands(const engine::FloatVec3 &direction, bool rotateToFaceDir = false);

    void dontShowModelInWorld();
    void setMovement(const engine::FloatVec3 &movement);
    void setMovement2D(const engine::FloatVec2 &movement);
    bool canJump() const;
    void jump();
    void onStep();
    bool rayTest_notMe(const engine::FloatVec3 &start, const engine::FloatVec3 &end, engine::app3D::CollisionFilter withWhatCollide, engine::FloatVec3 &outPos, int &outHitBodyUserIndex) const;

    CharacterDef &getDef() const;
    float getMsSinceLastTakenDamage() const;
    float getDistanceBetweenFeetAndEyes() const;
    bool isBusy() const;
    bool isSwimming() const;
    bool isUnderWater() const;
    bool wasWalkingPreviousFrame() const;
    int getEntityIDOnWhichWalkedPreviousFrame() const;
    bool isPlayer() const;
    bool isNPC() const;
    int getHP() const;
    ProjectilesSpreadAngleManager &getProjectilesSpreadAngleManager() const;
    engine::FloatVec3 getEyesPosition() const;
    engine::FloatVec3 getHorizontalLookVec() const;
    engine::FloatVec3 getCurrentVelocity() const;
    engine::FloatVec3 getNextItemUseSourcePos();
    Inventory &getInventory();
    const Inventory &getInventory() const;
    CharacterStatsAccumulator &getCharacterStatsAccumulator() const;
    const std::shared_ptr <CharacterStatsAccumulator> &getCharacterStatsAccumulatorPtr() const;
    PlayerComponent &getPlayerComponent() const;
    NPCComponent &getNPCComponent() const;

    ~Character();

private:
    using base = Entity;

    void updateSwimmingSounds();
    void updateDamageTimers();
    void updateRegenerateHP();
    void updateKillWhenTouchedWater();

    CharacterComponent &getCharacterComponent() const;
    void setBusyBecauseOfAnimation(bool busy);
    void decreaseHPBy(int by);
    void increaseHPBy(int by);
    void playBusyAnimation(const engine::IntRange &animation);
    bool isItThisPlayersCharacter() const;
    engine::FloatVec3 getFlyingAnimationOffset() const;
    void onKilled();
    void addCorpse();
    void addDynamicCharacterController();
    void makeSureCharacterControllerPosIsInWorldBounds();

    static const float k_minTimeBetweenSteps;
    static const float k_timeBetweenSwimmingForwardSounds;
    static const float k_timeBetweenSwimmingStillSounds;
    static const float k_defaultTimeBeforeDisposingBody;
    static const float k_startSwimmingSoundCooldownTime;
    static const float k_leaveWaterSoundCooldownTime;
    static const float k_expMultiplierForOwnerWhenKilledByStructure;
    static const float k_flyingAnimationOffsetElapsedMsSinMultiplier;
    static const float k_flyingAnimationOffsetAmplitude;
    static const engine::FloatRange k_damageDuringDayHoursRange;

    int m_nextItemUseSourcePosOffsetIndex;
    double m_lastTakenDamageTime;
    bool m_busyBecauseOfAnimation;
    bool m_dontShowModelInWorld;
    std::shared_ptr <CharacterDef> m_def;
    std::shared_ptr <engine::app3D::Model> m_characterModel;
    std::shared_ptr <engine::app3D::DynamicCharacterController> m_characterController;
    std::shared_ptr <engine::app3D::RigidBody> m_corpseRigidBody;
    std::shared_ptr <engine::app3D::Armature> m_armature;
    std::shared_ptr <CharacterStatsAccumulator> m_characterStatsAccumulator;
    Inventory m_inventory;
    std::unique_ptr <ProjectilesSpreadAngleManager> m_projectilesSpreadAngleManager;
    std::unique_ptr <PlayerComponent> m_playerComponent;
    std::unique_ptr <NPCComponent> m_NPCComponent;
    Timer m_stepTimer;
    bool m_previouslySwimming;
    bool m_previouslyUnderWater;
    Timer m_busyTimer;
    Timer m_swimmingSoundTimer;
    Timer m_disposeBodyTimer;
    Timer m_startSwimmingSoundCooldownTimer;
    Timer m_leaveWaterSoundCooldownTimer;

    Timer m_damageInWaterTimer;
    Timer m_damageDuringDayTimer;

    Timer m_regenerateHPPer5SecTimer;

    int m_HP;
    bool m_usePoofEffectOnRemovedFromWorld;
};

} // namespace app

#endif // APP_CHARACTER_HPP
