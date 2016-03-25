#ifndef APP_ITEM_CURRENTLY_DRAGGED_HPP
#define APP_ITEM_CURRENTLY_DRAGGED_HPP

#include "engine/util/Vec2.hpp"

#include <memory>

namespace app
{

class Item;
class IItemContainer;

class ItemCurrentlyDragged
{
public:
    ItemCurrentlyDragged();

    void setItem(const std::weak_ptr <Item> &item, const engine::IntVec2 &offset, const std::weak_ptr <IItemContainer> &itemContainer);
    void setItem(const std::weak_ptr <Item> &item, const engine::IntVec2 &offset, const std::weak_ptr <IItemContainer> &itemContainer, int draggedStack);
    void removeItem();
    void itemHasBeenMoved(bool movedWholeStack);
    bool hasItem() const;
    void draw() const;

    Item &getItem() const;
    std::shared_ptr <Item> getItemPtr() const;
    const engine::IntVec2 &getOffset() const;
    int getDraggedStack() const;

private:
    std::weak_ptr <Item> m_item;
    engine::IntVec2 m_offset;
    std::weak_ptr <IItemContainer> m_itemContainer;
    int m_draggedStack;
};

} // namespace app

#endif // APP_ITEM_CURRENTLY_DRAGGED_HPP
