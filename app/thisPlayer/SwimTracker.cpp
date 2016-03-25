#include "SwimTracker.hpp"

#include "engine/app3D/managers/ResourcesManager.hpp"
#include "engine/app3D/Device.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "../entities/Character.hpp"
#include "../defs/DefsCache.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../SoundPool.hpp"
#include "ThisPlayer.hpp"

namespace app
{

SwimTracker::SwimTracker()
    : m_previouslyUnderWater{},
      m_underWaterAmbience{Global::getCore().getDevice().getResourcesManager().getPathToResource(k_underWaterAmbiencePath)}
{
}

void SwimTracker::update()
{
    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    auto &soundPool = core.getSoundPool();
    auto &world = core.getWorld();
    const auto &character = core.getThisPlayer().getCharacter();

    bool underWater{character.isUnderWater()};

    if(underWater && !m_previouslyUnderWater) {
        soundPool.play(defsCache.Sound_GoUnderWater);
        world.playAmbientMusic(false);
        m_underWaterAmbience.play();
    }

    if(!underWater && m_previouslyUnderWater) {
        soundPool.play(defsCache.Sound_LeaveFromUnderWater);
        world.playAmbientMusic(true);
        m_underWaterAmbience.stop();
    }

    m_previouslyUnderWater = underWater;
}

const std::string SwimTracker::k_underWaterAmbiencePath = "music/underWater.ogg";

} // namespace app
