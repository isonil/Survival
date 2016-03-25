#include "StructureDef.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "EffectDef.hpp"

namespace app
{

ENUM_DEF(StructureDef::Category, Category,
    Wooden,
    Stone,
    Security,
    Misc
);

StructureDef::StructureDef()
    : m_canSnapToOtherStructures{},
      m_category{Category::Misc},
      m_maxHP{},
      m_requiredPower{},
      m_generatedPower{},
      m_hasSearchableItemContainer{},
      m_isWorkbench{}
{
}

void StructureDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_modelDef_defName, "modelDef");
    node.var(m_category, "category");
    node.var(m_canSnapToOtherStructures, "canSnapToOtherStructures", {});
    node.var(m_itemsWhenDeconstructed, "itemsWhenDeconstructed", {});
    node.var(m_maxHP, "maxHP");
    node.var(m_turretInfo, "turretInfo", {});
    node.var(m_requiredPower, "requiredPower", {});
    node.var(m_generatedPower, "generatedPower", {});
    node.var(m_hasSearchableItemContainer, "hasSearchableItemContainer", {});
    node.var(m_effectDef_defName, "effectDef", {});
    node.var(m_effectOffset, "effectOffset", {});
    node.var(m_isWorkbench, "isWorkbench", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::PostLoadInit) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_modelDef = defDatabase.getDef <engine::app3D::ModelDef> (m_modelDef_defName);

        if(m_maxHP <= 0)
            throw engine::Exception{"Max HP must be positive."};

        if(m_requiredPower < 0)
            throw engine::Exception{"Required power can't be negative."};

        if(m_generatedPower < 0)
            throw engine::Exception{"Generated power can't be negative."};

        if(!m_effectDef_defName.empty())
            m_effectDef = defDatabase.getDef <EffectDef> (m_effectDef_defName);
        else
            m_effectDef.reset();
    }
}

const engine::app3D::ModelDef &StructureDef::getModelDef() const
{
    if(!m_modelDef)
        throw engine::Exception{"Model def is nullptr."};

    return *m_modelDef;
}

const std::shared_ptr <engine::app3D::ModelDef> &StructureDef::getModelDefPtr() const
{
    if(!m_modelDef)
        throw engine::Exception{"Model def is nullptr."};

    return m_modelDef;
}

const StructureDef::Category &StructureDef::getCategory() const
{
    return m_category;
}

bool StructureDef::getCanSnapToOtherStructures() const
{
    return m_canSnapToOtherStructures;
}

const ItemsList &StructureDef::getItemsWhenDeconstructed() const
{
    return m_itemsWhenDeconstructed;
}

int StructureDef::getMaxHP() const
{
    return m_maxHP;
}

TurretInfo &StructureDef::getTurretInfo()
{
    return m_turretInfo;
}

const TurretInfo &StructureDef::getTurretInfo() const
{
    return m_turretInfo;
}

bool StructureDef::shouldExplodeWhenDestroyed() const
{
    return m_turretInfo.isTurret();
}

bool StructureDef::usesElectricity() const
{
    return m_requiredPower > 0 || m_generatedPower > 0;
}

int StructureDef::getRequiredPower() const
{
    return m_requiredPower;
}

int StructureDef::getGeneratedPower() const
{
    return m_generatedPower;
}

bool StructureDef::hasSearchableItemContainer() const
{
    return m_hasSearchableItemContainer;
}

bool StructureDef::hasEffectDef() const
{
    return static_cast <bool> (m_effectDef);
}

const std::shared_ptr <EffectDef> &StructureDef::getEffectDefPtr() const
{
    if(!m_effectDef)
        throw engine::Exception{"Effect def is nullptr. This should have been checked before."};

    return m_effectDef;
}

const engine::FloatVec3 &StructureDef::getEffectOffset() const
{
    return m_effectOffset;
}

bool StructureDef::isWorkbench() const
{
    return m_isWorkbench;
}

} // namespace app
