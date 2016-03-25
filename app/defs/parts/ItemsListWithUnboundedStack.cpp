#include "ItemsListWithUnboundedStack.hpp"

#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../ItemDef.hpp"

namespace app
{

ItemsListWithUnboundedStack::Element::Element()
    : m_count{1}
{
}

void ItemsListWithUnboundedStack::Element::expose(engine::DataFile::Node &node)
{
    node.var(m_itemDef_defName, "def");
    node.var(m_count, "count", 1);

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_itemDef = defDatabase.getDef <ItemDef> (m_itemDef_defName);

        if(m_count < 1)
            throw engine::Exception{"Count can't be less than 1."};
    }
}

ItemDef &ItemsListWithUnboundedStack::Element::getItemDef() const
{
    if(!m_itemDef)
        throw engine::Exception{"Item def is nullptr."};

    return *m_itemDef;
}

const std::shared_ptr <ItemDef> &ItemsListWithUnboundedStack::Element::getItemDefPtr() const
{
    if(!m_itemDef)
        throw engine::Exception{"Item def is nullptr."};

    return m_itemDef;
}

int ItemsListWithUnboundedStack::Element::getCount() const
{
    return m_count;
}

void ItemsListWithUnboundedStack::expose(engine::DataFile::Node &node)
{
    node.var(m_items, "items");
}

const std::vector <ItemsListWithUnboundedStack::Element> &ItemsListWithUnboundedStack::getItems() const
{
    return m_items;
}

} // namespace app
