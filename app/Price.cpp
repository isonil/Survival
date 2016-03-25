#include "Price.hpp"

#include "engine/util/DefDatabase.hpp"
#include "engine/util/RichText.hpp"
#include "entities/character/Inventory.hpp"
#include "defs/ItemDef.hpp"
#include "Global.hpp"
#include "Core.hpp"

namespace app
{

Price::RequiredItem::RequiredItem()
    : m_count{1}
{
}

void Price::RequiredItem::expose(engine::DataFile::Node &node)
{
    TRACK;

    node.var(m_itemDef_defName, "def");
    node.var(m_count, "count", 1);

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();
        m_itemDef = defDatabase.getDef <ItemDef> (m_itemDef_defName);

        if(m_count <= 0)
            throw engine::Exception{"Required item count must be a positive number."};
    }
}

ItemDef &Price::RequiredItem::getItemDef() const
{
    if(!m_itemDef)
        throw engine::Exception{"Item def is nullptr."};

    return *m_itemDef;
}

int Price::RequiredItem::getCount() const
{
    return m_count;
}

void Price::expose(engine::DataFile::Node &node)
{
    node.var(m_requiredItems, "items");
}

const std::vector <Price::RequiredItem> &Price::getRequiredItems() const
{
    return m_requiredItems;
}

int Price::getAllRequiredItemsTotalCount() const
{
    return std::accumulate(m_requiredItems.begin(), m_requiredItems.end(), 0, [](int current, const auto &elem) {
        return current + elem.getCount();
    });
}

std::string Price::toString() const
{
    std::ostringstream oss;

    auto first = true;

    for(const auto &elem : m_requiredItems) {
        if(!first)
            oss << ' ';
        else
            first = false;

        oss << elem.getItemDef().getLabel() << " x" << elem.getCount();
    }

    return oss.str();
}

engine::RichText Price::toRichText(const Inventory &inventory, char delim) const
{
    std::vector <bool> itemsAvailabilityStatus;

    inventory.canAfford(*this, &itemsAvailabilityStatus);

    E_DASSERT(itemsAvailabilityStatus.size() == m_requiredItems.size(), "Invalid size.");

    engine::RichText text;

    for(size_t i = 0; i < m_requiredItems.size(); ++i) {
        if(itemsAvailabilityStatus[i])
            text << engine::Color{165, 177, 253};
        else
            text << engine::Color{0.9f, 0.2f, 0.2f};

        text << m_requiredItems[i].getItemDef().getLabel();

        if(m_requiredItems[i].getCount() != 1)
            text << " x" << m_requiredItems[i].getCount();

        if(i != m_requiredItems.size() - 1)
            text << delim;
    }

    return text;
}

} // namespace app
