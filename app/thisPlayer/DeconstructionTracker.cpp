#include "DeconstructionTracker.hpp"

#include "engine/app3D/managers/EventManager.hpp"
#include "engine/app3D/Device.hpp"
#include "../entities/Entity.hpp"
#include "../world/World.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ThisPlayer.hpp"

namespace app
{

void DeconstructionTracker::update()
{
    TRACK;

    if(!isDeconstructingAnything())
        return;

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();
    const auto &pointedEntity = thisPlayer.getPointedEntity();

    if(!pointedEntity.isAny()) {
        stopDeconstructing();
        return;
    }

    const auto &eventManager = core.getDevice().getEventManager();

    if(!eventManager.isKeyPressed(k_deconstructKey)) {
        stopDeconstructing();
        return;
    }

    const auto &deconstructedEntityShared = m_deconstructedEntity.lock();

    E_DASSERT(deconstructedEntityShared, "Entity is nullptr (should have been already checked).");

    if(&pointedEntity.getEntity() != deconstructedEntityShared.get()) {
        stopDeconstructing();
        return;
    }

    auto &character = thisPlayer.getCharacter();

    if(!deconstructedEntityShared->canBeDeconstructed(character)) {
        stopDeconstructing();
        return;
    }

    if(m_deconstructionTimer.passed()) {
        deconstructedEntityShared->onDeconstructed(character);
        core.getWorld().removeEntity(deconstructedEntityShared->getEntityID());
        stopDeconstructing();
    }
}

void DeconstructionTracker::startDeconstructing(const std::weak_ptr <Entity> &entity)
{
    TRACK;

    const auto &entityShared = entity.lock();

    if(!entityShared)
        throw engine::Exception{"Can't start deconstructing nullptr entity."};

    const auto &thisPlayer = Global::getCore().getThisPlayer();
    const auto &character = thisPlayer.getCharacter();
    const auto &pointedEntity = thisPlayer.getPointedEntity();

    if(entityShared->canBeDeconstructed(character) && pointedEntity.isAny() && &pointedEntity.getEntity() == entityShared.get()) {
        m_deconstructedEntity = entity;
        m_deconstructionTimer.set(k_deconstructionDuration);
    }
}

void DeconstructionTracker::stopDeconstructing()
{
    m_deconstructedEntity.reset();
}

bool DeconstructionTracker::isDeconstructingAnything() const
{
    return !m_deconstructedEntity.expired();
}

float DeconstructionTracker::getDeconstructionProgress() const
{
    if(!isDeconstructingAnything())
        return 0.f;

    return m_deconstructionTimer.getProgress();
}

const float DeconstructionTracker::k_deconstructionDuration{2000.f};

} // namespace app
