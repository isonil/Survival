#include "EffectDef.hpp"

#include "engine/app3D/defs/ParticlesGroupDef.hpp"
#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/defs/LightDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

EffectDef::ParticleEffect::ParticleEffect()
    : m_startOffset{},
      m_duration{},
      m_lastsForever{}
{
}

void EffectDef::ParticleEffect::expose(engine::DataFile::Node &node)
{
    node.var(m_particlesGroupDef_defName, "particlesGroupDef");
    node.var(m_startOffset, "startOffset", {});
    node.var(m_duration, "duration", {});
    node.var(m_lastsForever, "lastsForever", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_particlesGroupDef = defDatabase.getDef <engine::app3D::ParticlesGroupDef> (m_particlesGroupDef_defName);

        if(m_startOffset < 0.f)
            throw engine::Exception{"Start offset can't be negative."};

        if(m_duration < 0.f)
            throw engine::Exception{"Duration can't be negative."};

        if(m_duration == 0.f && !m_lastsForever) {
            E_WARNING("EffectDef ParticleEffect has 0 duration.");
        }
    }
}

engine::app3D::ParticlesGroupDef &EffectDef::ParticleEffect::getParticlesGroupDef() const
{
    if(!m_particlesGroupDef)
        throw engine::Exception{"Particles group def is nullptr."};

    return *m_particlesGroupDef;
}

const std::shared_ptr <engine::app3D::ParticlesGroupDef> &EffectDef::ParticleEffect::getParticlesGroupDefPtr() const
{
    if(!m_particlesGroupDef)
        throw engine::Exception{"Particles group def is nullptr."};

    return m_particlesGroupDef;
}

float EffectDef::ParticleEffect::getStartOffset() const
{
    return m_startOffset;
}

float EffectDef::ParticleEffect::getDuration() const
{
    return m_duration;
}

bool EffectDef::ParticleEffect::lastsForever() const
{
    return m_lastsForever;
}

void EffectDef::expose(engine::DataFile::Node &node)
{
    base::expose(node);

    node.var(m_particleEffects, "particleEffects");
    node.var(m_soundDef_defName, "soundDef", {});
    node.var(m_lightDef_defName, "lightDef", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        if(!m_soundDef_defName.empty()) {
            m_soundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_soundDef_defName);

            if(m_soundDef->isGUISound()) {
                E_WARNING("EffectDef \"%s\" with GUI sound.", getDefName().c_str());
            }
        }
        else
            m_soundDef.reset();

        if(!m_lightDef_defName.empty())
            m_lightDef = defDatabase.getDef <engine::app3D::LightDef> (m_lightDef_defName);
        else
            m_lightDef.reset();
    }
}

const std::vector <EffectDef::ParticleEffect> &EffectDef::getParticleEffects() const
{
    return m_particleEffects;
}

bool EffectDef::hasSoundDef() const
{
    return static_cast <bool> (m_soundDef);
}

engine::app3D::SoundDef &EffectDef::getSoundDef() const
{
    if(!m_soundDef)
        throw engine::Exception{"Sound def is nullptr. This should have been checked before."};

    return *m_soundDef;
}

const std::shared_ptr <engine::app3D::SoundDef> &EffectDef::getSoundDefPtr() const
{
    if(!m_soundDef)
        throw engine::Exception{"Sound def is nullptr. This should have been checked before."};

    return m_soundDef;
}

bool EffectDef::hasLightDef() const
{
    return static_cast <bool> (m_lightDef);
}

const std::shared_ptr <engine::app3D::LightDef> &EffectDef::getLightDefPtr() const
{
    if(!m_lightDef)
        throw engine::Exception{"Light def is nullptr. This should have been checked before."};

    return m_lightDef;
}

bool EffectDef::lastsForever() const
{
    return std::any_of(m_particleEffects.begin(), m_particleEffects.end(), [](const auto &elem) {
        return elem.lastsForever();
    });
}

} // namespace app
