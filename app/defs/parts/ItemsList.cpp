#include "ItemsList.hpp"

#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../ItemDef.hpp"

namespace app
{

ItemsList::Element::Element()
    : m_stack{1}
{
}

void ItemsList::Element::expose(engine::DataFile::Node &node)
{
    node.var(m_itemDef_defName, "def");
    node.var(m_stack, "stack", 1);

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_itemDef = defDatabase.getDef <ItemDef> (m_itemDef_defName);

        if(m_stack < 1)
            throw engine::Exception{"Stack can't be less than 1."};

        if(m_stack > m_itemDef->getMaxStack())
            throw engine::Exception{"Stack can't be greater than max stack."};
    }
}

ItemDef &ItemsList::Element::getItemDef() const
{
    if(!m_itemDef)
        throw engine::Exception{"Item def is nullptr."};

    return *m_itemDef;
}

const std::shared_ptr <ItemDef> &ItemsList::Element::getItemDefPtr() const
{
    if(!m_itemDef)
        throw engine::Exception{"Item def is nullptr."};

    return m_itemDef;
}

int ItemsList::Element::getStack() const
{
    return m_stack;
}

void ItemsList::expose(engine::DataFile::Node &node)
{
    node.var(m_items, "items");
}

const std::vector <ItemsList::Element> &ItemsList::getItems() const
{
    return m_items;
}

} // namespace app
