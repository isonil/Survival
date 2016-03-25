#ifndef APP_ITEM_CONTAINER_SEARCH_TRACKER_HPP
#define APP_ITEM_CONTAINER_SEARCH_TRACKER_HPP

#include <irrlicht/Keycodes.h>

#include <memory>

namespace app
{

class Entity;

class ItemContainerSearchTracker
{
public:
    void update();

    bool isSearchingAnything() const;
    void startSearching(const std::weak_ptr <Entity> &entity);
    void stopSearching();
    Entity &getSearchedEntity() const;

    static constexpr irr::EKEY_CODE k_searchKey{irr::KEY_KEY_E};

private:
    static const float k_maxDistToContainer;

    std::weak_ptr <Entity> m_searchedEntity;
};

} // namespace app

#endif // APP_ITEM_CONTAINER_SEARCH_TRACKER_HPP
