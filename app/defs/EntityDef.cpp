#include "EntityDef.hpp"

#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "CachedCollisionShapeDef.hpp"
#include "EffectDef.hpp"

namespace app
{

EntityDef::EntityDef()
    : m_mass{}
{
}

void EntityDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_cachedCollisionShapeDef_defName, "cachedCollisionShapeDef");
    node.var(m_stepSoundDef_defName, "stepSoundDef", {});
    node.var(m_onHitEffectDef_defName, "onHitEffectDef", {});
    node.var(m_mass, "mass", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();
        m_cachedCollisionShapeDef = defDatabase.getDef <CachedCollisionShapeDef> (m_cachedCollisionShapeDef_defName);

        if(!m_stepSoundDef_defName.empty())
            m_stepSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_stepSoundDef_defName);
        else
            m_stepSoundDef.reset();

        if(!m_onHitEffectDef_defName.empty())
            m_onHitEffectDef = defDatabase.getDef <EffectDef> (m_onHitEffectDef_defName);
        else
            m_onHitEffectDef = defDatabase.getDef <EffectDef> ("Effect_OnHitGeneric");

        if(engine::Math::fuzzyCompare(m_mass, 0.f))
            m_mass = 0.f;

        if(m_mass < 0.f)
            throw engine::Exception{"Mass can't be negative."};
    }
}

CachedCollisionShapeDef &EntityDef::getCachedCollisionShapeDef() const
{
    if(!m_cachedCollisionShapeDef)
        throw engine::Exception{"Cached collision shape def is nullptr."};

    return *m_cachedCollisionShapeDef;
}

const std::shared_ptr <CachedCollisionShapeDef> &EntityDef::getCachedCollisionShapeDefPtr() const
{
    if(!m_cachedCollisionShapeDef)
        throw engine::Exception{"Cached collision shape def is nullptr."};

    return m_cachedCollisionShapeDef;
}

bool EntityDef::hasStepSound() const
{
    return static_cast <bool> (m_stepSoundDef);
}

const std::shared_ptr <engine::app3D::SoundDef> &EntityDef::getStepSoundDefPtr() const
{
    if(!m_stepSoundDef)
        throw engine::Exception{"Sound def is nullptr. This should have been checked before."};

    return m_stepSoundDef;
}

EffectDef &EntityDef::getOnHitEffectDef() const
{
    if(!m_onHitEffectDef)
        throw engine::Exception{"Effect def is nullptr."};

    return *m_onHitEffectDef;
}

const std::shared_ptr <EffectDef> &EntityDef::getOnHitEffectDefPtr() const
{
    if(!m_onHitEffectDef)
        throw engine::Exception{"Effect def is nullptr."};

    return m_onHitEffectDef;
}

float EntityDef::getMass() const
{
    return m_mass;
}

bool EntityDef::hasMass() const
{
    return !engine::Math::fuzzyCompare(m_mass, 0.f);
}

} // namespace app
