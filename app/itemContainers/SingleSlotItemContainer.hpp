#ifndef APP_SINGLE_SLOT_ITEM_CONTAINER_HPP
#define APP_SINGLE_SLOT_ITEM_CONTAINER_HPP

#include "engine/util/Enum.hpp"
#include "IItemContainer.hpp"

namespace app
{

class CharacterStatsAccumulator;

class SingleSlotItemContainer : public IItemContainer
{
public:
    ENUM_DECL(SlotType,
        None,
        Hands,
        Armor,
        Special);

    SingleSlotItemContainer();
    SingleSlotItemContainer(const SlotType &slotType, const std::shared_ptr <CharacterStatsAccumulator> &characterStatsAccumulator);

    int tryAddItem(std::shared_ptr <Item> item) override;
    int tryAddItem(std::shared_ptr <Item> item, int stack) override;
    bool tryRemoveItem(int itemEntityID) override;

    const SlotType &getSlotType() const;
    bool wouldFit(const Item &item) const;
    bool hasItem() const;
    Item &getItem() const;
    const std::shared_ptr <Item> &getItemPtr() const;

private:
    SlotType m_slotType;
    std::shared_ptr <CharacterStatsAccumulator> m_characterStatsAccumulator;
    std::shared_ptr <Item> m_item;
};

} // namespace app

#endif // APP_SINGLE_SLOT_ITEM_CONTAINER_HPP
