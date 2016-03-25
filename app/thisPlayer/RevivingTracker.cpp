#include "RevivingTracker.hpp"

#include "engine/app3D/managers/EventManager.hpp"
#include "../entities/Entity.hpp"
#include "engine/app3D/Device.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ThisPlayer.hpp"

namespace app
{

void RevivingTracker::update()
{
    TRACK;

    if(!isRevivingAnything())
        return;

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();
    const auto &pointedEntity = thisPlayer.getPointedEntity();

    if(!pointedEntity.isAny()) {
        stopReviving();
        return;
    }

    const auto &eventManager = core.getDevice().getEventManager();

    if(!eventManager.isKeyPressed(k_reviveKey)) {
        stopReviving();
        return;
    }

    const auto &revivedEntityShared = m_revivedEntity.lock();

    E_DASSERT(revivedEntityShared, "Entity is nullptr (should have been already checked).");

    if(&pointedEntity.getEntity() != revivedEntityShared.get()) {
        stopReviving();
        return;
    }

    auto &character = thisPlayer.getCharacter();

    if(!revivedEntityShared->canBeRevived(character)) {
        stopReviving();
        return;
    }

    if(m_revivingTimer.passed()) {
        revivedEntityShared->onRevived(character);
        stopReviving();
    }
}

void RevivingTracker::startReviving(const std::weak_ptr <Entity> &entity)
{
    TRACK;

    const auto &entityShared = entity.lock();

    if(!entityShared)
        throw engine::Exception{"Can't start reviving nullptr entity."};

    const auto &thisPlayer = Global::getCore().getThisPlayer();
    const auto &character = thisPlayer.getCharacter();
    const auto &pointedEntity = thisPlayer.getPointedEntity();

    if(entityShared->canBeRevived(character) && pointedEntity.isAny() && &pointedEntity.getEntity() == entityShared.get()) {
        m_revivedEntity = entity;
        m_revivingTimer.set(k_revivingDuration);
    }
}

void RevivingTracker::stopReviving()
{
    m_revivedEntity.reset();
}

bool RevivingTracker::isRevivingAnything() const
{
    return !m_revivedEntity.expired();
}

float RevivingTracker::getRevivingProgress() const
{
    if(!isRevivingAnything())
        return 0.f;

    return m_revivingTimer.getProgress();
}

const float RevivingTracker::k_revivingDuration{2000.f};

} // namespace app
