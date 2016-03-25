#include "Effect.hpp"

#include "engine/app3D/sceneNodes/Light.hpp"
#include "engine/app3D/defs/ParticlesGroupDef.hpp"
#include "engine/app3D/particles/ParticlesGroup.hpp"
#include "engine/app3D/managers/ParticlesManager.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "../defs/EffectDef.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/app3D/Sound.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

Effect::Effect(const std::shared_ptr <EffectDef> &def, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot)
    : m_def{def},
      m_pos{pos},
      m_rot{rot},
      m_startTime{}
{
    if(!m_def)
        throw engine::Exception{"Effect def is nullptr."};

    auto &core = Global::getCore();
    auto &particlesManager = core.getDevice().getParticlesManager();

    m_startTime = core.getAppTime().getElapsedMs();

    for(const auto &elem : m_def->getParticleEffects()) {
        if(engine::Math::fuzzyCompare(elem.getStartOffset(), 0.f)) {
            const auto &particlesGroup = particlesManager.addParticlesGroup(elem.getParticlesGroupDefPtr());

            particlesGroup->setPosition(m_pos);
            particlesGroup->setRotation(m_rot);

            m_currentParticlesGroups.emplace_back();
            m_currentParticlesGroups.back().particlesGroup = particlesGroup;
            m_currentParticlesGroups.back().endTime = m_startTime + elem.getDuration();
            m_currentParticlesGroups.back().lastsForever = elem.lastsForever();
        }
        else {
            m_pendingParticlesGroups.emplace_back();
            m_pendingParticlesGroups.back().particlesGroupDef = elem.getParticlesGroupDefPtr();
            m_pendingParticlesGroups.back().startTime = m_startTime + elem.getStartOffset();
            m_pendingParticlesGroups.back().endTime = m_startTime + elem.getStartOffset() + elem.getDuration();
            m_pendingParticlesGroups.back().lastsForever = elem.lastsForever();
        }
    }

    if(m_def->hasSoundDef()) {
        m_sound = std::make_unique <engine::app3D::Sound> (m_def->getSoundDefPtr());
        m_sound->setPosition(pos);

        if(m_def->lastsForever())
            m_sound->fadeInLooped();
        else
            m_sound->play();
    }

    if(m_def->hasLightDef()) {
        m_light = core.getDevice().getSceneManager().addLight(m_def->getLightDefPtr());
        m_light->setPosition(m_pos);
    }
}

void Effect::update()
{
    auto &core = Global::getCore();
    auto &particlesManager = core.getDevice().getParticlesManager();

    double curTime{core.getAppTime().getElapsedMs()};

    E_DASSERT(m_def, "Effect def is nullptr.");

    for(size_t i = 0; i < m_pendingParticlesGroups.size();) {
        if(curTime >= m_pendingParticlesGroups[i].startTime) {
            m_currentParticlesGroups.emplace_back();

            m_currentParticlesGroups.back().particlesGroup = particlesManager.addParticlesGroup(m_pendingParticlesGroups[i].particlesGroupDef);
            m_currentParticlesGroups.back().endTime = m_pendingParticlesGroups[i].endTime;
            m_currentParticlesGroups.back().lastsForever = m_pendingParticlesGroups[i].lastsForever;

            std::swap(m_pendingParticlesGroups[i], m_pendingParticlesGroups.back());
            m_pendingParticlesGroups.pop_back();
        }
        else
            ++i;
    }

    for(size_t i = 0; i < m_currentParticlesGroups.size();) {
        E_DASSERT(m_currentParticlesGroups[i].particlesGroup, "Particles group is nullptr.");

        bool removed{};

        if(curTime >= m_currentParticlesGroups[i].endTime && !m_currentParticlesGroups[i].lastsForever) {
            m_currentParticlesGroups[i].particlesGroup->stopAddingNewParticles();

            if(!m_currentParticlesGroups[i].particlesGroup->anyVisibleParticle()) {
                std::swap(m_currentParticlesGroups[i], m_currentParticlesGroups.back());
                m_currentParticlesGroups.pop_back();

                removed = true;
            }
        }

        if(!removed) {
            m_currentParticlesGroups[i].particlesGroup->setPosition(m_pos);
            m_currentParticlesGroups[i].particlesGroup->setRotation(m_rot);

            ++i;
        }
    }

    if(m_sound)
        m_sound->update(core.getAppTime());

    if(m_light)
        m_light->setPosition(m_pos);
}

void Effect::setPosition(const engine::FloatVec3 &pos)
{
    m_pos = pos;
}

void Effect::setRotation(const engine::FloatVec3 &rot)
{
    m_rot = rot;
}

void Effect::setAllCurrentZonesToBox(const engine::FloatVec3 &dimension)
{
    for(auto &elem : m_currentParticlesGroups) {
        E_DASSERT(elem.particlesGroup, "Particles group is nullptr.");
        return elem.particlesGroup->setAllZonesToBox(dimension);
    }
}

void Effect::setAllCurrentZonesToSphere(float radius)
{
    for(auto &elem : m_currentParticlesGroups) {
        E_DASSERT(elem.particlesGroup, "Particles group is nullptr.");
        return elem.particlesGroup->setAllZonesToSphere(radius);
    }
}

void Effect::stop()
{
    m_pendingParticlesGroups.clear();

    // we can't just remove all particle groups, because we need to wait
    // until all currently visible particles slowly disappear

    for(auto &elem : m_currentParticlesGroups) {
        E_DASSERT(elem.particlesGroup, "Particles group is nullptr.");

        elem.particlesGroup->stopAddingNewParticles();
        elem.endTime = 0.0;
        elem.lastsForever = false;
    }

    if(m_sound)
        m_sound->fadeOut();
}

bool Effect::ended() const
{
    auto soundStopped = true;

    if(m_sound)
        soundStopped = m_sound->stopped();

    return m_currentParticlesGroups.empty() && m_pendingParticlesGroups.empty() && soundStopped;
}

Effect::~Effect()
{
}

} // namespace app
