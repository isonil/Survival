#include "UnlockedStructures.hpp"

#include "engine/util/DefDatabase.hpp"
#include "../../../defs/StructureRecipeDef.hpp"
#include "../../../Global.hpp"
#include "../../../Core.hpp"

namespace app
{

UnlockedStructures::UnlockedStructures()
{
    unlockUnlockedByDefault();
}

void UnlockedStructures::unlock(const std::shared_ptr <StructureRecipeDef> &def)
{
    if(!def)
        throw engine::Exception{"Tried to unlock nullptr structure recipe def."};

    if(std::find(m_unlocked.begin(), m_unlocked.end(), def) != m_unlocked.end())
        return;

    m_unlocked.push_back(def);
}

const std::vector <std::shared_ptr <StructureRecipeDef>> &UnlockedStructures::getUnlocked() const
{
    return m_unlocked;
}

void UnlockedStructures::unlockUnlockedByDefault()
{
    E_DASSERT(m_unlocked.empty(), "This method does not check duplicates, so it requires 0 unlocked structure defs yet.");

    const auto &defDatabase = Global::getCore().getDefDatabase();
    const auto &allDefs = defDatabase.getDefs <StructureRecipeDef> ();

    for(const auto &elem : allDefs) {
        if(elem.second->isUnlockedByDefault())
            unlock(elem.second);
    }
}

} // namespace app
