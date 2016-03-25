#include "ItemGathersResourcesProperties.hpp"

namespace app
{

ItemGathersResourcesProperties::ItemGathersResourcesProperties()
    : m_gatheredCount{}
{
}

void ItemGathersResourcesProperties::expose(engine::DataFile::Node &node)
{
    TRACK;

    node.var(m_mineableTags, "mineableTags");
    node.var(m_gatheredCount, "gatheredCount", {});
    node.var(m_decreaseDurabilityBy, "decreaseDurabilityBy", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        if(m_gatheredCount < 0)
            throw engine::Exception{"Gathered count can't be negative."};

        if(m_decreaseDurabilityBy < 0)
            throw engine::Exception{"Decrease durability by value can't be negative."};
    }
}

bool ItemGathersResourcesProperties::gathersAnything() const
{
    return m_gatheredCount > 0 && !m_mineableTags.empty();
}

const std::vector <std::string> &ItemGathersResourcesProperties::getMineableTags() const
{
    return m_mineableTags;
}

int ItemGathersResourcesProperties::getGatheredCount() const
{
    return m_gatheredCount;
}

int ItemGathersResourcesProperties::getDecreaseDurabilityBy() const
{
    return m_decreaseDurabilityBy;
}

} // namespace app
