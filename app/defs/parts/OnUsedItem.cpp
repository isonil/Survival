#include "OnUsedItem.hpp"

#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../EffectDef.hpp"
#include "../AnimationFramesSetDef.hpp"

namespace app
{

OnUsedItem::UseAnimation::UseAnimation()
    : m_FPPAnimationIndex{},
      m_timeBetweenUseAndActualEffects{}
{
}

void OnUsedItem::UseAnimation::expose(engine::DataFile::Node &node)
{
    node.var(m_FPPAnimationIndex, "FPPAnimationIndex", {});
    node.var(m_timeBetweenUseAndActualEffects, "timeBetweenUseAndActualEffects", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        if(m_FPPAnimationIndex < 0)
            throw engine::Exception{"FPP animation index can't be negative."};

        if(m_FPPAnimationIndex >= AnimationFramesSetDef::k_maxUseAnimations)
            throw engine::Exception{"Only " + std::to_string(AnimationFramesSetDef::k_maxUseAnimations) + " FPP use animations are allowed."};

        if(m_timeBetweenUseAndActualEffects < 0.f)
            throw engine::Exception{"Time between use and actual effects can't be negative."};
    }
}

int OnUsedItem::UseAnimation::getFPPAnimationIndex() const
{
    return m_FPPAnimationIndex;
}

float OnUsedItem::UseAnimation::getTimeBetweenUseAndActualEffects() const
{
    return m_timeBetweenUseAndActualEffects;
}

OnUsedItem::OnUsedItem()
    : m_minDuration{},
      m_continuousUse{},
      m_isMelee{true},
      m_dealsDamage{},
      m_recoil{},
      m_minProjectilesSpreadAngle{}
{
}

void OnUsedItem::expose(engine::DataFile::Node &node)
{
    TRACK;

    node.var(m_soundDef_defName, "soundDef", {});
    node.var(m_minDuration, "minDuration");
    node.var(m_continuousUse, "continuousUse", {});
    node.var(m_isMelee, "isMelee");
    node.var(m_dealsDamage, "dealsDamage", {});
    node.var(m_recoil, "recoil", {});
    node.var(m_minProjectilesSpreadAngle, "minProjectilesSpreadAngle", {});
    node.var(m_useAnimations, "useAnimations");
    node.var(m_gathersResourcesProperties, "gathersResources", {});
    node.var(m_effectDef_defName, "effectDef", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        if(m_minDuration < 0.f)
            throw engine::Exception{"Min duration can't be negative."};

        if(m_dealsDamage < 0)
            throw engine::Exception{"Damage can't be negative."};

        if(m_recoil < 0.f)
            throw engine::Exception{"Recoil can't be negative."};

        if(m_minProjectilesSpreadAngle < 0.f)
            throw engine::Exception{"Min projectiles spread angle can't be negative."};

        if(m_useAnimations.empty())
            throw engine::Exception{"There must be at least one use animation."};

        if(!m_soundDef_defName.empty())
            m_soundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_soundDef_defName);
        else
            m_soundDef.reset();

        if(!m_effectDef_defName.empty())
            m_effectDef = defDatabase.getDef <EffectDef> (m_effectDef_defName);
        else
            m_effectDef.reset();
    }
}

bool OnUsedItem::hasSoundDef() const
{
    return static_cast <bool> (m_soundDef);
}

const std::shared_ptr <engine::app3D::SoundDef> &OnUsedItem::getSoundDefPtr() const
{
    if(!m_soundDef)
        throw engine::Exception{"Sound def is nullptr. This should have been checked before."};

    return m_soundDef;
}

float OnUsedItem::getMinDuration() const
{
    return m_minDuration;
}

bool OnUsedItem::isContinuousUse() const
{
    return m_continuousUse;
}

bool OnUsedItem::isWeapon() const
{
    return m_dealsDamage > 0;
}

bool OnUsedItem::isMelee() const
{
    return m_isMelee;
}

int OnUsedItem::getDealsDamage() const
{
    return m_dealsDamage;
}

float OnUsedItem::getRecoil() const
{
    if(isMelee())
        return 0.f;

    return m_recoil;
}

float OnUsedItem::getMinProjectilesSpreadAngle() const
{
    if(isMelee())
        return 0.f;

    return m_minProjectilesSpreadAngle;
}

int OnUsedItem::getRandomUseAnimationIndex(int tryNotToTakeThisIndex) const
{
    if(m_useAnimations.empty())
        throw engine::Exception{"There are no any use animations."};

    if(m_useAnimations.size() == 1)
        return 0;

    static std::vector <int> indices;

    indices.clear();
    indices.reserve(m_useAnimations.size());

    for(size_t i = 0; i < m_useAnimations.size(); ++i) {
        if(static_cast <int> (i) != tryNotToTakeThisIndex)
            indices.push_back(i);
    }

    return indices[engine::Random::rangeExclusive(0, indices.size())];
}

const std::vector <OnUsedItem::UseAnimation> &OnUsedItem::getUseAnimations() const
{
    return m_useAnimations;
}

const ItemGathersResourcesProperties &OnUsedItem::getGathersResourcesProperties() const
{
    return m_gathersResourcesProperties;
}

bool OnUsedItem::hasEffectDef() const
{
    return static_cast <bool> (m_effectDef);
}

EffectDef &OnUsedItem::getEffectDef() const
{
    if(!m_effectDef)
        throw engine::Exception{"Effect def is nullptr. This should have been checked before."};

    return *m_effectDef;
}

const std::shared_ptr <EffectDef> &OnUsedItem::getEffectDefPtr() const
{
    if(!m_effectDef)
        throw engine::Exception{"Effect def is nullptr. This should have been checked before."};

    return m_effectDef;
}

} // namespace app
