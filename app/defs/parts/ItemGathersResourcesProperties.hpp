#ifndef APP_ITEM_GATHERS_RESOURCES_PROPERTIES_HPP
#define APP_ITEM_GATHERS_RESOURCES_PROPERTIES_HPP

#include "engine/util/DataFile.hpp"

namespace app
{

class ItemGathersResourcesProperties : public engine::DataFile::Saveable
{
public:
    ItemGathersResourcesProperties();

    void expose(engine::DataFile::Node &node) override;

    bool gathersAnything() const;
    const std::vector <std::string> &getMineableTags() const;
    int getGatheredCount() const;
    int getDecreaseDurabilityBy() const;

private:
    int m_gatheredCount;
    int m_decreaseDurabilityBy;
    std::vector <std::string> m_mineableTags;
};

} // namespace app

#endif // APP_ITEM_GATHERS_RESOURCES_PROPERTIES_HPP
