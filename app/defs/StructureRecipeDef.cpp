#include "StructureRecipeDef.hpp"

#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "StructureDef.hpp"
#include "ItemDef.hpp"

namespace app
{

StructureRecipeDef::StructureRecipeDef()
    : m_isUnlockedByDefault{},
      m_constructingExpForConstructing{}
{
}

void StructureRecipeDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_structureDef_defName, "structureDef");
    node.var(m_isUnlockedByDefault, "isUnlockedByDefault");
    node.var(m_constructingExpForConstructing, "constructingExpForConstructing");
    node.var(m_price, "price");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();
        m_structureDef = defDatabase.getDef <StructureDef> (m_structureDef_defName);

        if(m_constructingExpForConstructing < 0)
            throw engine::Exception{"Constructing exp for constructing can't be negative."};
    }
}

StructureDef &StructureRecipeDef::getStructureDef() const
{
    if(!m_structureDef)
        throw engine::Exception{"Structure def is nullptr."};

    return *m_structureDef;
}

const std::shared_ptr <StructureDef> &StructureRecipeDef::getStructureDefPtr() const
{
    if(!m_structureDef)
        throw engine::Exception{"Structure def is nullptr."};

    return m_structureDef;
}

bool StructureRecipeDef::isUnlockedByDefault() const
{
    return m_isUnlockedByDefault;
}

const Price &StructureRecipeDef::getPrice() const
{
    return m_price;
}

int StructureRecipeDef::getConstructingExpForConstructing() const
{
    return m_constructingExpForConstructing;
}

} // namespace app
