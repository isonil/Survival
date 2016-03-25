#ifndef APP_FACTION_DEF_HPP
#define APP_FACTION_DEF_HPP

#include "engine/util/Def.hpp"
#include "engine/util/Trace.hpp"
#include "FactionRelationDef.hpp"

namespace app
{

class FactionDef : public engine::Def, public engine::Tracked <FactionDef>
{
public:
    FactionRelationDef::Relation getRelation(const FactionDef &with) const;
};

} // namespace app

#endif // APP_FACTION_DEF_HPP

