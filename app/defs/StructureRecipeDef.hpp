#ifndef APP_STRUCTURE_RECIPE_DEF_HPP
#define APP_STRUCTURE_RECIPE_DEF_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Def.hpp"
#include "../Price.hpp"

namespace app
{

class StructureDef;

class StructureRecipeDef : public engine::Def, public engine::Tracked <StructureRecipeDef>
{
public:
    StructureRecipeDef();

    void expose(engine::DataFile::Node &node) override;

    StructureDef &getStructureDef() const;
    const std::shared_ptr <StructureDef> &getStructureDefPtr() const;
    bool isUnlockedByDefault() const;
    const Price &getPrice() const;
    int getConstructingExpForConstructing() const;

private:
    using base = Def;

    std::string m_structureDef_defName;
    std::shared_ptr <StructureDef> m_structureDef;
    bool m_isUnlockedByDefault;
    int m_constructingExpForConstructing;
    Price m_price;
};

} // namespace app

#endif // APP_STRUCTURE_RECIPE_DEF_HPP
