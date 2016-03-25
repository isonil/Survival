#ifndef APP_STRUCTURE_DEF_HPP
#define APP_STRUCTURE_DEF_HPP

#include "engine/util/Enum.hpp"
#include "parts/ItemsList.hpp"
#include "parts/TurretInfo.hpp"
#include "EntityDef.hpp"

#include <memory>

namespace engine { namespace app3D { class ModelDef; } }

namespace app
{

class StructureDef : public EntityDef, public engine::Tracked <StructureDef>
{
public:
    ENUM_DECL(Category,
        Wooden,
        Stone,
        Security,
        Misc
    );

    StructureDef();

    void expose(engine::DataFile::Node &node) override;

    const engine::app3D::ModelDef &getModelDef() const;
    const std::shared_ptr <engine::app3D::ModelDef> &getModelDefPtr() const;

    const Category &getCategory() const;
    bool getCanSnapToOtherStructures() const;
    const ItemsList &getItemsWhenDeconstructed() const;
    int getMaxHP() const;
    TurretInfo &getTurretInfo();
    const TurretInfo &getTurretInfo() const;
    bool shouldExplodeWhenDestroyed() const;
    bool usesElectricity() const;
    int getRequiredPower() const;
    int getGeneratedPower() const;
    bool hasSearchableItemContainer() const;
    bool hasEffectDef() const;
    const std::shared_ptr <EffectDef> &getEffectDefPtr() const;
    const engine::FloatVec3 &getEffectOffset() const;
    bool isWorkbench() const;

private:
    using base = EntityDef;

    bool m_canSnapToOtherStructures;
    std::string m_modelDef_defName;
    std::shared_ptr <engine::app3D::ModelDef> m_modelDef;
    Category m_category;
    ItemsList m_itemsWhenDeconstructed;
    int m_maxHP;
    TurretInfo m_turretInfo;
    int m_requiredPower;
    int m_generatedPower;
    bool m_hasSearchableItemContainer;
    std::string m_effectDef_defName;
    std::shared_ptr <EffectDef> m_effectDef;
    engine::FloatVec3 m_effectOffset;
    bool m_isWorkbench;
};

} // namespace app

#endif // APP_STRUCTURE_DEF_HPP
