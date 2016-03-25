#include "FactionRelationDef.hpp"

#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "FactionDef.hpp"

namespace app
{

ENUM_DEF(FactionRelationDef::Relation, Relation,
    Neutral,
    Good,
    Hostile);

FactionRelationDef::FactionRelationDef()
    : m_relation{Relation::Neutral}
{
}

void FactionRelationDef::expose(engine::DataFile::Node &node)
{
    base::expose(node);

    node.var(m_firstFactionDef_defName, "firstFactionDef");
    node.var(m_secondFactionDef_defName, "secondFactionDef");
    node.var(m_relation, "relation");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_firstFactionDef = defDatabase.getDef <FactionDef> (m_firstFactionDef_defName);
        m_secondFactionDef = defDatabase.getDef <FactionDef> (m_secondFactionDef_defName);
    }
}

FactionDef &FactionRelationDef::getFirstFactionDef() const
{
    if(!m_firstFactionDef)
        throw engine::Exception{"Faction def is nullptr."};

    return *m_firstFactionDef;
}

FactionDef &FactionRelationDef::getSecondFactionDef() const
{
    if(!m_secondFactionDef)
        throw engine::Exception{"Faction def is nullptr."};

    return *m_secondFactionDef;
}

FactionRelationDef::Relation FactionRelationDef::getRelation() const
{
    return m_relation;
}

} // namespace app
