#include "EffectsPool.hpp"

#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/app3D/Sound.hpp"
#include "defs/EffectDef.hpp"
#include "Global.hpp"
#include "Core.hpp"

namespace app
{

void EffectsPool::update()
{
    for(auto it = m_effects.begin(); it != m_effects.end();) {
        if(it->ended())
            it = m_effects.erase(it);
        else {
            it->update();
            ++it;
        }
    }
}

bool EffectsPool::add(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos)
{
    return add(effectDef, pos, {});
}

bool EffectsPool::add(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot)
{
    if(!effectDef)
        throw engine::Exception{"Tried to add nullptr effect def."};

    const auto &cameraPos = Global::getCore().getDevice().getSceneManager().getCameraPosition();

    if(cameraPos.getDistanceSq(pos) > engine::app3D::SceneManager::k_cameraFarValue * engine::app3D::SceneManager::k_cameraFarValue)
        return false; // we don't want to show particle effects if they are very far

    if(effectDef->lastsForever()) {
        E_WARNING("Tried to add never-ending effect def to effects pool. Not the best idea.");
        return false;
    }

    m_effects.push_back(Effect{effectDef, pos, rot});

    return true;
}

bool EffectsPool::add_boxZone(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot, const engine::FloatVec3 &boxDimension)
{
    if(!add(effectDef, pos, rot))
        return false;

    E_DASSERT(!m_effects.empty(), "No added effect.");

    m_effects.back().setAllCurrentZonesToBox(boxDimension);

    return true;
}

bool EffectsPool::add_sphereZone(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot, float radius)
{
    if(!add(effectDef, pos, rot))
        return false;

    E_DASSERT(!m_effects.empty(), "No added effect.");

    m_effects.back().setAllCurrentZonesToSphere(radius);

    return true;
}

} // namespace app
