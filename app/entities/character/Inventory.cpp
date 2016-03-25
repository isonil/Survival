#include "Inventory.hpp"

#include "../../itemContainers/MultiSlotItemContainer.hpp"
#include "../../itemContainers/SingleSlotItemContainer.hpp"
#include "../../Price.hpp"
#include "../Item.hpp"

namespace app
{

Inventory::Inventory(const std::shared_ptr <CharacterStatsAccumulator> &characterStatsAccumulator)
    : m_characterStatsAccumulator{characterStatsAccumulator},
      m_multiSlotItemContainer{std::make_shared <MultiSlotItemContainer> (k_multiSlotItemContainerSize)},
      m_handsItemContainer{std::make_shared <SingleSlotItemContainer> (SingleSlotItemContainer::SlotType::Hands, characterStatsAccumulator)},
      m_armorItemContainer{std::make_shared <SingleSlotItemContainer> (SingleSlotItemContainer::SlotType::Armor, characterStatsAccumulator)},
      m_specialItemContainer{std::make_shared <SingleSlotItemContainer> (SingleSlotItemContainer::SlotType::Special, characterStatsAccumulator)}
{
    if(!m_characterStatsAccumulator)
        throw engine::Exception{"Character stats accumultor is nullptr."};
}

MultiSlotItemContainer &Inventory::getMultiSlotItemContainer() const
{
    E_DASSERT(m_multiSlotItemContainer, "Multi slot item container is nullptr.");
    return *m_multiSlotItemContainer;
}

const std::shared_ptr <MultiSlotItemContainer> &Inventory::getMultiSlotItemContainerPtr() const
{
    E_DASSERT(m_multiSlotItemContainer, "Multi slot item container is nullptr.");
    return m_multiSlotItemContainer;
}

SingleSlotItemContainer &Inventory::getHandsItemContainer() const
{
    E_DASSERT(m_handsItemContainer, "Single slot item container is nullptr.");
    return *m_handsItemContainer;
}

const std::shared_ptr <SingleSlotItemContainer> &Inventory::getHandsItemContainerPtr() const
{
    E_DASSERT(m_handsItemContainer, "Single slot item container is nullptr.");
    return m_handsItemContainer;
}

SingleSlotItemContainer &Inventory::getArmorItemContainer() const
{
    E_DASSERT(m_armorItemContainer, "Single slot item container is nullptr.");
    return *m_armorItemContainer;
}

const std::shared_ptr <SingleSlotItemContainer> &Inventory::getArmorItemContainerPtr() const
{
    E_DASSERT(m_armorItemContainer, "Single slot item container is nullptr.");
    return m_armorItemContainer;
}

SingleSlotItemContainer &Inventory::getSpecialItemContainer() const
{
    E_DASSERT(m_specialItemContainer, "Single slot item container is nullptr.");
    return *m_specialItemContainer;
}

const std::shared_ptr <SingleSlotItemContainer> &Inventory::getSpecialItemContainerPtr() const
{
    E_DASSERT(m_specialItemContainer, "Single slot item container is nullptr.");
    return m_specialItemContainer;
}

bool Inventory::canAfford(const Price &price, std::vector <bool> *outItemsAvailabilityStatus) const
{
    TRACK;

    E_DASSERT(m_multiSlotItemContainer, "Multi slot item container is nullptr.");

    if(outItemsAvailabilityStatus)
        outItemsAvailabilityStatus->clear();

    auto canAfford = true;

    const auto &availableItems = m_multiSlotItemContainer->getItems();

    for(const auto &elem : price.getRequiredItems()) {
        int count{std::accumulate(availableItems.begin(), availableItems.end(), 0, [&elem](int sum, const MultiSlotItemContainer::ItemInContainer &item) {
            E_DASSERT(item.item, "Item is nullptr.");

            if(&item.item->getDef() == &elem.getItemDef())
                return sum + item.item->getStack();
            else
                return sum;
        })};

        if(outItemsAvailabilityStatus) {
            outItemsAvailabilityStatus->push_back(count >= elem.getCount());

            if(count < elem.getCount())
                canAfford = false;
        }
        else if(count < elem.getCount())
            return false;
    }

    return canAfford;
}

void Inventory::pay(const Price &price)
{
    TRACK;

    E_DASSERT(m_multiSlotItemContainer, "Multi slot item container is nullptr.");

    for(const auto &elem : price.getRequiredItems()) {
        int leftNeeded{elem.getCount()};

        while(leftNeeded > 0) {
            bool foundAnything{};

            auto &availableItems = m_multiSlotItemContainer->getItems();

            for(auto &item : availableItems) {
                E_DASSERT(item.item, "Item is nullptr.");

                if(&item.item->getDef() == &elem.getItemDef()) {
                    foundAnything = true;

                    int stack{item.item->getStack()};

                    E_DASSERT(stack > 0, "Stack is <= 0."); // protection in case infinite loop bug

                    if(stack <= leftNeeded) {
                        m_multiSlotItemContainer->tryRemoveItem(item.item->getEntityID());
                        leftNeeded -= stack;
                        break; // we have to break, because availableItems has been invalidated
                    }
                    else {
                        item.item->split(leftNeeded);
                        leftNeeded = 0;
                        break; // we break, because we found needed items
                    }
                }
            }

            if(!foundAnything)
                break;
        }

        if(leftNeeded > 0) {
            E_WARNING("Could not pay price because not all items were available. This should have been checked before.");
        }
    }
}

const engine::IntVec2 Inventory::k_multiSlotItemContainerSize{6, 9};

} // namespace app
