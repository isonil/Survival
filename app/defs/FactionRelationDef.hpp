#ifndef APP_FACTION_RELATION_DEF_HPP
#define APP_FACTION_RELATION_DEF_HPP

#include "engine/util/Def.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/Enum.hpp"

namespace app
{

class FactionDef;

class FactionRelationDef : public engine::Def, public engine::Tracked <FactionRelationDef>
{
public:
    ENUM_DECL(Relation,
        Neutral,
        Good,
        Hostile);

    FactionRelationDef();

    void expose(engine::DataFile::Node &node) override;

    FactionDef &getFirstFactionDef() const;
    FactionDef &getSecondFactionDef() const;
    Relation getRelation() const;

private:
    using base = Def;

    std::shared_ptr <FactionDef> m_firstFactionDef;
    std::shared_ptr <FactionDef> m_secondFactionDef;
    std::string m_firstFactionDef_defName;
    std::string m_secondFactionDef_defName;
    Relation m_relation;
};

} // namespace app

#endif // APP_FACTION_RELATION_DEF_HPP

