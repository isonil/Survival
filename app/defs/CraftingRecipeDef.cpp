#include "CraftingRecipeDef.hpp"

#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ItemDef.hpp"
#include "StructureDef.hpp"

namespace app
{

CraftingRecipeDef::CraftingRecipeDef()
    : m_craftedItemStack{1}
{
}

void CraftingRecipeDef::expose(engine::DataFile::Node &node)
{
    base::expose(node);

    node.var(m_requiredWorkbenchStructureDef_defName, "requiredWorkbenchStructureDef", {});
    node.var(m_price, "price");
    node.var(m_craftedItemDef_defName, "craftedItemDef");
    node.var(m_craftedItemStack, "craftedItemStack", 1);
    node.var(m_skillsRequirement, "skillsRequirement", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        if(!m_requiredWorkbenchStructureDef_defName.empty())
            m_requiredWorkbenchStructureDef = defDatabase.getDef <StructureDef> (m_requiredWorkbenchStructureDef_defName);
        else
            m_requiredWorkbenchStructureDef.reset();

        m_craftedItemDef = defDatabase.getDef <ItemDef> (m_craftedItemDef_defName);

        if(m_craftedItemStack < 1)
            throw engine::Exception{"Crafted item stack must be at least 1."};

        if(m_craftedItemStack > m_craftedItemDef->getMaxStack())
            throw engine::Exception{"Crafted item stack can't be greater than max stack."};
    }
}

bool CraftingRecipeDef::requiresWorkbench() const
{
    return static_cast <bool> (m_requiredWorkbenchStructureDef);
}

StructureDef &CraftingRecipeDef::getRequiredWorkbenchStructureDef() const
{
    if(!m_requiredWorkbenchStructureDef)
        throw engine::Exception{"Required workbench structure def is nullptr. This should have been checked before."};

    return *m_requiredWorkbenchStructureDef;
}

const std::shared_ptr <StructureDef> &CraftingRecipeDef::getRequiredWorkbenchStructureDefPtr() const
{
    if(!m_requiredWorkbenchStructureDef)
        throw engine::Exception{"Required workbench structure def is nullptr. This should have been checked before."};

    return m_requiredWorkbenchStructureDef;
}

const Price &CraftingRecipeDef::getPrice() const
{
    return m_price;
}

ItemDef &CraftingRecipeDef::getCraftedItemDef() const
{
    if(!m_craftedItemDef)
        throw engine::Exception{"Crafted item def is nullptr."};

    return *m_craftedItemDef;
}

const std::shared_ptr <ItemDef> &CraftingRecipeDef::getCraftedItemDefPtr() const
{
    if(!m_craftedItemDef)
        throw engine::Exception{"Crafted item def is nullptr."};

    return m_craftedItemDef;
}

int CraftingRecipeDef::getCraftedItemStack() const
{
    return m_craftedItemStack;
}

const SkillsRequirement &CraftingRecipeDef::getSkillsRequirement() const
{
    return m_skillsRequirement;
}

} // namespace app
