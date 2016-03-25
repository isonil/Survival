#include "SingleSlotItemContainer.hpp"

#include "../entities/character/CharacterStatsAccumulator.hpp"
#include "../entities/Item.hpp"
#include "../defs/ItemDef.hpp"

namespace app
{

ENUM_DEF(SingleSlotItemContainer::SlotType, SlotType,
    None,
    Hands,
    Armor,
    Special);

SingleSlotItemContainer::SingleSlotItemContainer()
    : m_slotType{SlotType::None}
{
}

SingleSlotItemContainer::SingleSlotItemContainer(const SlotType &slotType, const std::shared_ptr<CharacterStatsAccumulator> &characterStatsAccumulator)
    : m_slotType{slotType},
      m_characterStatsAccumulator{characterStatsAccumulator}
{
    if(!m_characterStatsAccumulator)
        throw engine::Exception{"Character stats accumulator is nullptr."};
}

int SingleSlotItemContainer::tryAddItem(std::shared_ptr <Item> item)
{
    TRACK;

    if(!item)
        throw engine::Exception{"Can't insert nullptr item."};

    return tryAddItem(item, item->getStack());
}

int SingleSlotItemContainer::tryAddItem(std::shared_ptr <Item> item, int stack)
{
    TRACK;

    if(!item)
        throw engine::Exception{"Can't insert nullptr item."};

    if(stack <= 0)
        throw engine::Exception{"Can't insert <= 0 stack."};

    if(stack > item->getStack())
        throw engine::Exception{"Can't insert bigger stack than there are actual items."};

    if(m_slotType != item->getDef().getSlotType())
        return 0;

    if(!m_item) {
        if(item->getStack() == stack)
            m_item = item;
        else
            m_item = item->split(stack);

        if(m_characterStatsAccumulator)
            m_characterStatsAccumulator->add(m_item->getDef().getCharacterStatsChange());

        return stack;
    }
    else if(m_item != item && m_item->getDefPtr() == item->getDefPtr())
        return m_item->tryMergeStack(*item, stack);

    return 0;
}

bool SingleSlotItemContainer::tryRemoveItem(int itemEntityID)
{
    TRACK;

    if(m_item && m_item->getEntityID() == itemEntityID) {
        if(m_characterStatsAccumulator)
            m_characterStatsAccumulator->remove(m_item->getDef().getCharacterStatsChange());

        m_item.reset();
        return true;
    }

    return false;
}

const SingleSlotItemContainer::SlotType &SingleSlotItemContainer::getSlotType() const
{
    return m_slotType;
}

bool SingleSlotItemContainer::wouldFit(const Item &item) const
{
    if(m_slotType != item.getDef().getSlotType())
        return false;

    if(!m_item)
        return true;

    // can't insert the same item
    if(m_item.get() == &item)
        return false;

    return m_item->getDefPtr() == item.getDefPtr() && !m_item->hasFullStack();
}

bool SingleSlotItemContainer::hasItem() const
{
    return static_cast <bool> (m_item);
}

Item &SingleSlotItemContainer::getItem() const
{
    if(!m_item)
        throw engine::Exception{"Tried to get nullptr item from single slot item container. This should have been checked before."};

    return *m_item;
}

const std::shared_ptr <Item> &SingleSlotItemContainer::getItemPtr() const
{
    if(!m_item)
        throw engine::Exception{"Tried to get nullptr item from single slot item container. This should have been checked before."};

    return m_item;
}

} // namespace app
