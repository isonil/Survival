#ifndef APP_CHARACTER_DEF_HPP
#define APP_CHARACTER_DEF_HPP

#include "../entities/components/player/SkillsRequirement.hpp"
#include "engine/util/Enum.hpp"
#include "engine/util/Vec3.hpp"
#include "EntityDef.hpp"

#include <memory>

namespace engine { namespace app3D { class ModelDef; } }

namespace app
{

class AnimationFramesSetDef;

class CharacterDef : public EntityDef, public engine::Tracked <CharacterDef>
{
public:
    ENUM_DECL(OnKilledAction,
        None,
        MakeBodyDynamic);

    class OnKilledEffect : public engine::DataFile::Saveable
    {
    public:
        OnKilledEffect();

        void expose(engine::DataFile::Node &node) override;

        EffectDef &getEffectDef() const;
        const std::shared_ptr <EffectDef> &getEffectDefPtr() const;
        float getDisposeBodyTime() const;

    private:
        std::string m_effectDef_defName;
        std::shared_ptr <EffectDef> m_effectDef;
        float m_disposeBodyTime;
    };

    CharacterDef();

    void expose(engine::DataFile::Node &node) override;

    const engine::app3D::ModelDef &getModelDef() const;
    const std::shared_ptr <engine::app3D::ModelDef> &getModelDefPtr() const;

    const std::vector <engine::FloatVec3> &getItemUseSourcePosOffsets() const;

    bool hasOnSpawnedSoundDef() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getOnSpawnedSoundDefPtr() const;

    const AnimationFramesSetDef &getAnimationFramesSetDef() const;

    bool canFly() const;
    int getMaxHP() const;
    int getDamageInWaterPer5Seconds() const;
    int getDamageDuringDayPer5Seconds() const;
    int getExpPerKill() const;
    OnKilledAction getOnKilledAction() const;
    bool hasAnyOnKilledEffect() const;
    const OnKilledEffect &getRandomOnKilledEffect() const;
    bool hasAnyNonInstantOnKilledEffect() const;
    const OnKilledEffect &getRandomNonInstantOnKilledEffect() const;
    bool killWhenTouchedWater() const;
    bool canBeRevived() const;
    const SkillsRequirement &getSkillsRequiredToRevive() const;
    int getElectronicsExpForReviving() const;
    bool shouldEverDisappearAfterBeingKilled() const;

private:
    using base = EntityDef;

    std::string m_modelDef_defName;
    std::string m_animationFramesSetDef_defName;
    std::shared_ptr <engine::app3D::ModelDef> m_modelDef;
    std::shared_ptr <AnimationFramesSetDef> m_animationFramesSetDef;
    std::string m_onSpawnedSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_onSpawnedSoundDef;
    std::vector <engine::FloatVec3> m_itemUseSourcePosOffsets;
    bool m_canFly;
    int m_maxHP;
    int m_damageInWaterPer5Seconds;
    int m_damageDuringDayPer5Seconds;
    int m_expPerKill;
    OnKilledAction m_onKilledAction;
    std::vector <OnKilledEffect> m_onKilledEffects;
    std::vector <OnKilledEffect> m_nonInstantOnKilledEffects;
    bool m_killWhenTouchedWater;
    bool m_canBeRevived;
    SkillsRequirement m_skillsRequiredToRevive;
    int m_electronicsExpForReviving;
};

} // namespace app

#endif // APP_CHARACTER_DEF_HPP
