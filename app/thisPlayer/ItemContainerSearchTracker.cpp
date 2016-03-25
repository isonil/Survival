#include "ItemContainerSearchTracker.hpp"

#include "../entities/Character.hpp"
#include "../entities/Entity.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ThisPlayer.hpp"

namespace app
{

void ItemContainerSearchTracker::update()
{
    if(m_searchedEntity.expired())
        return;

    const auto &shared = m_searchedEntity.lock();

    if(!shared->hasSearchableItemContainer()) {
        stopSearching();
        return;
    }

    if(!shared->isInWorld()) {
        stopSearching();
        return;
    }

    const auto &playerPos = Global::getCore().getThisPlayer().getCharacter().getInWorldPosition();

    if(shared->getInWorldPosition().getDistanceSq(playerPos) > k_maxDistToContainer * k_maxDistToContainer)
        stopSearching();
}

bool ItemContainerSearchTracker::isSearchingAnything() const
{
    return !m_searchedEntity.expired();
}

void ItemContainerSearchTracker::startSearching(const std::weak_ptr <Entity> &entity)
{
    if(entity.expired())
        return;

    const auto &shared = entity.lock();

    if(!shared->hasSearchableItemContainer())
        return;

    if(!shared->isInWorld())
        return;

    const auto &playerPos = Global::getCore().getThisPlayer().getCharacter().getInWorldPosition();

    if(shared->getInWorldPosition().getDistanceSq(playerPos) > k_maxDistToContainer * k_maxDistToContainer)
        return;

    m_searchedEntity = entity;
}

void ItemContainerSearchTracker::stopSearching()
{
    m_searchedEntity.reset();
}

Entity &ItemContainerSearchTracker::getSearchedEntity() const
{
    const auto &shared = m_searchedEntity.lock();

    if(!shared)
        throw engine::Exception{"Searched entity is nullptr. This should have been checked before."};

    return *shared;
}

const float ItemContainerSearchTracker::k_maxDistToContainer{7.f};

} // namespace app
