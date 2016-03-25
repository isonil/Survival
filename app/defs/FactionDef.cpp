#include "FactionDef.hpp"

#include "../Global.hpp"
#include "../Core.hpp"
#include "DefsCache.hpp"

namespace app
{

FactionRelationDef::Relation FactionDef::getRelation(const FactionDef &with) const
{
    if(this == &with)
        return FactionRelationDef::Relation::Good;

    const auto &defsCache = Global::getCore().getDefsCache();

    for(const auto &elem : defsCache.AllFactionRelations) {
        if((&elem->getFirstFactionDef() == this && &elem->getSecondFactionDef() == &with) ||
           (&elem->getFirstFactionDef() == &with && &elem->getSecondFactionDef() == this))
            return elem->getRelation();
    }

    return FactionRelationDef::Relation::Neutral;
}

} // namespace app
