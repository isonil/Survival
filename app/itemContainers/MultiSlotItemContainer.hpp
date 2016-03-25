#ifndef APP_MULTI_SLOT_ITEM_CONTAINER_HPP
#define APP_MULTI_SLOT_ITEM_CONTAINER_HPP

#include "engine/util/Rect.hpp"
#include "engine/util/Vec2.hpp"
#include "IItemContainer.hpp"

namespace app
{

class MultiSlotItemContainer : public IItemContainer
{
public:
    struct ItemInContainer
    {
        engine::IntRect getRect() const;

        std::shared_ptr <Item> item;
        engine::IntVec2 position;
    };

    MultiSlotItemContainer(const engine::IntVec2 &size);

    int tryAddItem(std::shared_ptr <Item> item) override;
    int tryAddItem(std::shared_ptr <Item> item, int stack) override;
    bool tryRemoveItem(int itemEntityID) override;

    int tryAddItem(std::shared_ptr <Item> item, const engine::IntVec2 &pos);
    int tryAddItem(std::shared_ptr <Item> item, const engine::IntVec2 &pos, int stack);
    std::shared_ptr <Item> tryGetItemAt(const engine::IntVec2 &pos) const;
    bool wouldFit(const Item &item, const engine::IntVec2 &pos) const;
    bool wouldFit(const engine::IntRect &rect) const;

    const std::vector <ItemInContainer> &getItems() const;
    const engine::IntVec2 &getSize() const;

private:
    std::experimental::optional <engine::IntVec2> getFreeSpacePosition(const engine::IntVec2 &size);

    engine::IntVec2 m_size;
    std::vector <ItemInContainer> m_items;
};

} // namespace app

#endif // APP_MULTI_SLOT_ITEM_CONTAINER_HPP
