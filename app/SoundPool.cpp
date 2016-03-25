#include "SoundPool.hpp"

#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/Device.hpp"
#include "entities/Entity.hpp"
#include "Global.hpp"
#include "Core.hpp"

namespace app
{

void SoundPool::update()
{
    for(auto it = m_sounds.begin(); it != m_sounds.end();) {
        if(it->sound.stopped())
            it = m_sounds.erase(it);
        else {
            if(it->followEntity) {
                const auto &entityShared = it->entity.lock();

                if(entityShared)
                    it->sound.setPosition(entityShared->getInWorldPosition());
            }
            ++it;
        }
    }
}

void SoundPool::play(const std::shared_ptr <engine::app3D::SoundDef> &soundDef)
{
    if(!soundDef)
        throw engine::Exception{"Tried to play nullptr sound def."};

    if(!soundDef->isGUISound()) {
        E_WARNING("Added non-GUI sound to SoundPool without specified position.");
    }

    m_sounds.emplace_back(engine::app3D::Sound{soundDef});
    m_sounds.back().sound.play();
}

void SoundPool::play(const std::shared_ptr <engine::app3D::SoundDef> &soundDef, const engine::FloatVec3 &pos)
{
    if(!soundDef)
        throw engine::Exception{"Tried to play nullptr sound def."};

    if(soundDef->isGUISound()) {
        E_WARNING("Added GUI sound to SoundPool with specified position.");
    }

    const auto &cameraPos = Global::getCore().getDevice().getSceneManager().getCameraPosition();

    if(cameraPos.getDistanceSq(pos) > engine::app3D::SceneManager::k_cameraFarValue * engine::app3D::SceneManager::k_cameraFarValue)
        return; // we don't want to play sounds if they are very far

    m_sounds.emplace_back(engine::app3D::Sound{soundDef});
    m_sounds.back().sound.setPosition(pos);
    m_sounds.back().sound.play();
}

void SoundPool::play(const std::shared_ptr <engine::app3D::SoundDef> &soundDef, const std::weak_ptr <Entity> &entity)
{
    const auto &entityShared = entity.lock();

    if(!entityShared) {
        play(soundDef);
        return;
    }

    if(soundDef->isGUISound()) {
        E_WARNING("Added GUI sound to SoundPool with specified position.");
    }

    const auto &entityInWorldPos = entityShared->getInWorldPosition();
    const auto &cameraPos = Global::getCore().getDevice().getSceneManager().getCameraPosition();

    if(cameraPos.getDistanceSq(entityInWorldPos) > engine::app3D::SceneManager::k_cameraFarValue * engine::app3D::SceneManager::k_cameraFarValue)
        return; // we don't want to play sounds if they are very far

    m_sounds.emplace_back(engine::app3D::Sound{soundDef}, entity);
    m_sounds.back().sound.setPosition(entityInWorldPos);
    m_sounds.back().sound.play();
}

SoundPool::SoundWithInfo::SoundWithInfo(engine::app3D::Sound &&sound)
    : sound{std::move(sound)},
      followEntity{}
{
}

SoundPool::SoundWithInfo::SoundWithInfo(engine::app3D::Sound &&sound, const std::weak_ptr <Entity> &entity)
    : sound{std::move(sound)},
      followEntity{true},
      entity{entity}
{
}

} // namespace app
