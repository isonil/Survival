#ifndef APP_IITEM_CONTAINER_HPP
#define APP_IITEM_CONTAINER_HPP

#include "engine/util/Trace.hpp"

#include <memory>

namespace app
{

class Item;

class IItemContainer : public engine::Tracked <IItemContainer>
{
public:
    virtual int tryAddItem(std::shared_ptr <Item> item) = 0;
    virtual int tryAddItem(std::shared_ptr <Item> item, int stack) = 0;
    virtual bool tryRemoveItem(int itemEntityID) = 0;
};

} // namespace app

#endif // APP_IITEM_CONTAINER_HPP
