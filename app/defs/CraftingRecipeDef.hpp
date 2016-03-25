#ifndef APP_CRAFTING_RECIPE_DEF_HPP
#define APP_CRAFTING_RECIPE_DEF_HPP

#include "../entities/components/player/SkillsRequirement.hpp"
#include "engine/util/Def.hpp"
#include "../Price.hpp"

namespace app
{

class ItemDef;
class StructureDef;

class CraftingRecipeDef : public engine::Def
{
public:
    CraftingRecipeDef();

    void expose(engine::DataFile::Node &node) override;

    bool requiresWorkbench() const;
    StructureDef &getRequiredWorkbenchStructureDef() const;
    const std::shared_ptr <StructureDef> &getRequiredWorkbenchStructureDefPtr() const;

    const Price &getPrice() const;

    ItemDef &getCraftedItemDef() const;
    const std::shared_ptr <ItemDef> &getCraftedItemDefPtr() const;
    int getCraftedItemStack() const;

    const SkillsRequirement &getSkillsRequirement() const;

private:
    using base = Def;

    std::string m_requiredWorkbenchStructureDef_defName;
    std::shared_ptr <StructureDef> m_requiredWorkbenchStructureDef;

    Price m_price;

    std::string m_craftedItemDef_defName;
    std::shared_ptr <ItemDef> m_craftedItemDef;
    int m_craftedItemStack;

    SkillsRequirement m_skillsRequirement;
};

} // namespace app

#endif // APP_CRAFTING_RECIPE_DEF_HPP
