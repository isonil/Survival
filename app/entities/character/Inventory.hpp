#ifndef APP_INVENTORY_HPP
#define APP_INVENTORY_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Vec2.hpp"

#include <memory>

namespace app
{

class MultiSlotItemContainer;
class SingleSlotItemContainer;
class CharacterStatsAccumulator;
class Price;

class Inventory : public engine::Tracked <Inventory>
{
public:
    Inventory(const std::shared_ptr <CharacterStatsAccumulator> &characterStatsAccumulator);

    MultiSlotItemContainer &getMultiSlotItemContainer() const;
    const std::shared_ptr <MultiSlotItemContainer> &getMultiSlotItemContainerPtr() const;

    SingleSlotItemContainer &getHandsItemContainer() const;
    const std::shared_ptr <SingleSlotItemContainer> &getHandsItemContainerPtr() const;

    SingleSlotItemContainer &getArmorItemContainer() const;
    const std::shared_ptr <SingleSlotItemContainer> &getArmorItemContainerPtr() const;

    SingleSlotItemContainer &getSpecialItemContainer() const;
    const std::shared_ptr <SingleSlotItemContainer> &getSpecialItemContainerPtr() const;

    bool canAfford(const Price &price, std::vector <bool> *outItemsAvailabilityStatus = {}) const;
    void pay(const Price &price);

private:
    static const engine::IntVec2 k_multiSlotItemContainerSize;

    std::shared_ptr <CharacterStatsAccumulator> m_characterStatsAccumulator;

    std::shared_ptr <MultiSlotItemContainer> m_multiSlotItemContainer;
    std::shared_ptr <SingleSlotItemContainer> m_handsItemContainer;
    std::shared_ptr <SingleSlotItemContainer> m_armorItemContainer;
    std::shared_ptr <SingleSlotItemContainer> m_specialItemContainer;
};

} // namespace app

#endif // APP_INVENTORY_HPP
