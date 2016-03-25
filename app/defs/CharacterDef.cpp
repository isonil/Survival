#include "CharacterDef.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../defs/AnimationFramesSetDef.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "EffectDef.hpp"

namespace app
{

ENUM_DEF(CharacterDef::OnKilledAction, OnKilledAction,
    None,
    MakeBodyDynamic);

CharacterDef::OnKilledEffect::OnKilledEffect()
    : m_disposeBodyTime{}
{
}

void CharacterDef::OnKilledEffect::expose(engine::DataFile::Node &node)
{
    node.var(m_effectDef_defName, "effectDef");
    node.var(m_disposeBodyTime, "disposeBodyTime", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_effectDef = defDatabase.getDef <EffectDef> (m_effectDef_defName);

        if(m_disposeBodyTime < 0.f)
            throw engine::Exception{"Dispose body time can't be negative."};
    }
}

EffectDef &CharacterDef::OnKilledEffect::getEffectDef() const
{
    if(!m_effectDef)
        throw engine::Exception{"Effect def is nullptr."};

    return *m_effectDef;
}

const std::shared_ptr <EffectDef> &CharacterDef::OnKilledEffect::getEffectDefPtr() const
{
    if(!m_effectDef)
        throw engine::Exception{"Effect def is nullptr."};

    return m_effectDef;
}

float CharacterDef::OnKilledEffect::getDisposeBodyTime() const
{
    return m_disposeBodyTime;
}

CharacterDef::CharacterDef()
    : m_canFly{},
      m_maxHP{},
      m_damageInWaterPer5Seconds{},
      m_damageDuringDayPer5Seconds{},
      m_expPerKill{},
      m_onKilledAction{OnKilledAction::None},
      m_killWhenTouchedWater{},
      m_canBeRevived{},
      m_electronicsExpForReviving{}
{
}

void CharacterDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_modelDef_defName, "modelDef");
    node.var(m_animationFramesSetDef_defName, "animationFramesSetDef");
    node.var(m_onSpawnedSoundDef_defName, "onSpawnedSoundDef", {});
    node.var(m_itemUseSourcePosOffsets, "itemUseSourcePosOffsets");
    node.var(m_canFly, "canFly", {});
    node.var(m_maxHP, "maxHP");
    node.var(m_damageInWaterPer5Seconds, "damageInWaterPer5Seconds", {});
    node.var(m_damageDuringDayPer5Seconds, "damageDuringDayPer5Seconds", {});
    node.var(m_expPerKill, "expPerKill");
    node.var(m_onKilledAction, "onKilledAction");
    node.var(m_onKilledEffects, "onKilledEffects");
    node.var(m_killWhenTouchedWater, "killWhenTouchedWater", {});
    node.var(m_canBeRevived, "canBeRevived", {});
    node.var(m_skillsRequiredToRevive, "skillsRequiredToRevive", {});
    node.var(m_electronicsExpForReviving, "m_electronicsExpForReviving", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_modelDef = defDatabase.getDef <engine::app3D::ModelDef> (m_modelDef_defName);
        m_animationFramesSetDef = defDatabase.getDef <AnimationFramesSetDef> (m_animationFramesSetDef_defName);

        if(!m_onSpawnedSoundDef_defName.empty())
            m_onSpawnedSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_onSpawnedSoundDef_defName);
        else
            m_onSpawnedSoundDef.reset();

        if(m_maxHP <= 0)
            throw engine::Exception{"Max HP must be positive."};

        if(m_damageInWaterPer5Seconds < 0)
            throw engine::Exception{"Damage in water per second is negative."};

        if(m_damageDuringDayPer5Seconds < 0)
            throw engine::Exception{"Damage during day per second is negative."};

        if(m_expPerKill < 0)
            throw engine::Exception{"Exp per kill can't be negative."};

        m_nonInstantOnKilledEffects.clear();

        for(const auto &elem : m_onKilledEffects) {
            if(elem.getDisposeBodyTime() > 0.f)
                m_nonInstantOnKilledEffects.emplace_back(elem);
        }

        if(m_electronicsExpForReviving < 0)
            throw engine::Exception{"Electronics exp can't be negative."};
    }
}

const engine::app3D::ModelDef &CharacterDef::getModelDef() const
{
    if(!m_modelDef)
        throw engine::Exception{"Model def is nullptr."};

    return *m_modelDef;
}

const std::shared_ptr <engine::app3D::ModelDef> &CharacterDef::getModelDefPtr() const
{
    if(!m_modelDef)
        throw engine::Exception{"Model def is nullptr."};

    return m_modelDef;
}

const std::vector <engine::FloatVec3> &CharacterDef::getItemUseSourcePosOffsets() const
{
    return m_itemUseSourcePosOffsets;
}

bool CharacterDef::hasOnSpawnedSoundDef() const
{
    return static_cast <bool> (m_onSpawnedSoundDef);
}

const std::shared_ptr <engine::app3D::SoundDef> &CharacterDef::getOnSpawnedSoundDefPtr() const
{
    if(!m_onSpawnedSoundDef)
        throw engine::Exception{"On spawned sound def is nullptr. This should have been checked before."};

    return m_onSpawnedSoundDef;
}

const AnimationFramesSetDef &CharacterDef::getAnimationFramesSetDef() const
{
    if(!m_animationFramesSetDef)
        throw engine::Exception{"Animation frames set def is nullptr."};

    return *m_animationFramesSetDef;
}

bool CharacterDef::canFly() const
{
    return m_canFly;
}

int CharacterDef::getMaxHP() const
{
    return m_maxHP;
}

int CharacterDef::getDamageInWaterPer5Seconds() const
{
    return m_damageInWaterPer5Seconds;
}

int CharacterDef::getDamageDuringDayPer5Seconds() const
{
    return m_damageDuringDayPer5Seconds;
}

int CharacterDef::getExpPerKill() const
{
    return m_expPerKill;
}

CharacterDef::OnKilledAction CharacterDef::getOnKilledAction() const
{
    return m_onKilledAction;
}

bool CharacterDef::hasAnyOnKilledEffect() const
{
    return !m_onKilledEffects.empty();
}

const CharacterDef::OnKilledEffect &CharacterDef::getRandomOnKilledEffect() const
{
    if(m_onKilledEffects.empty())
        throw engine::Exception{"Tried to get random on killed effect but there is none. This should have been checked before."};

    return m_onKilledEffects[engine::Random::rangeExclusive(0, m_onKilledEffects.size())];
}

bool CharacterDef::hasAnyNonInstantOnKilledEffect() const
{
    return !m_nonInstantOnKilledEffects.empty();
}

const CharacterDef::OnKilledEffect &CharacterDef::getRandomNonInstantOnKilledEffect() const
{
    if(m_nonInstantOnKilledEffects.empty())
        throw engine::Exception{"Tried to get random non instant on killed effect but there is none. This should have been checked before."};

    return m_nonInstantOnKilledEffects[engine::Random::rangeExclusive(0, m_nonInstantOnKilledEffects.size())];
}

bool CharacterDef::killWhenTouchedWater() const
{
    return m_killWhenTouchedWater;
}

bool CharacterDef::canBeRevived() const
{
    return m_canBeRevived;
}

const SkillsRequirement &CharacterDef::getSkillsRequiredToRevive() const
{
    return m_skillsRequiredToRevive;
}

int CharacterDef::getElectronicsExpForReviving() const
{
    return m_electronicsExpForReviving;
}

bool CharacterDef::shouldEverDisappearAfterBeingKilled() const
{
    return !m_canBeRevived;
}

} // namespace app
