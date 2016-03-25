#include "UnlockedUpgrades.hpp"

#include "../../../defs/UpgradeDef.hpp"
#include "../../../Core.hpp"
#include "../../../Global.hpp"
#include "../../Character.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../PlayerComponent.hpp"
#include "UnlockedStructures.hpp"

namespace app
{

UnlockedUpgrades::UnlockedUpgrades(Character &character, PlayerComponent &playerComponent)
    : m_character{character},
      m_playerComponent{playerComponent},
      m_upgradePoints{}
{
    unlockUnlockedByDefault();
}

bool UnlockedUpgrades::tryUnlock(const std::shared_ptr <UpgradeDef> &upgradeDef, bool removeUpgradePoints)
{
    if(!upgradeDef)
        throw engine::Exception{"Tried to unlock nullptr upgrade def."};

    if(isUnlocked(upgradeDef))
        return false;

    if(removeUpgradePoints && m_upgradePoints < upgradeDef->getRequiredUpgradePoints())
        return false;

    m_upgradePoints -= upgradeDef->getRequiredUpgradePoints();

    m_unlockedUpgradeDefs.insert(upgradeDef);

    m_character.getCharacterStatsAccumulator().add(upgradeDef->getCharacterStatsChange());

    auto &unlockedStructures = m_playerComponent.getUnlockedStructures();

    for(const auto &elem : upgradeDef->getUnlockStructureRecipeDefs()) {
        unlockedStructures.unlock(elem);
    }

    return true;
}

bool UnlockedUpgrades::isUnlocked(const std::shared_ptr <UpgradeDef> &upgradeDef) const
{
    return m_unlockedUpgradeDefs.find(upgradeDef) != m_unlockedUpgradeDefs.end();
}

bool UnlockedUpgrades::isUnlocked(const UpgradeDef &upgradeDef) const
{
    return std::any_of(m_unlockedUpgradeDefs.begin(), m_unlockedUpgradeDefs.end(), [&upgradeDef](const auto &elem) {
        return elem.get() == &upgradeDef;
    });
}

int UnlockedUpgrades::getUpgradePoints() const
{
    return m_upgradePoints;
}

void UnlockedUpgrades::addUpgradePoints(int points)
{
    if(points <= 0)
        return;

    m_upgradePoints += points;
}

void UnlockedUpgrades::unlockUnlockedByDefault()
{
    auto &defDatabase = Global::getCore().getDefDatabase();

    for(const auto &elem : defDatabase.getDefs <UpgradeDef> ()) {
        E_DASSERT(elem.second, "Upgrade def is nullptr.");

        if(elem.second->isUnlockedByDefault())
            tryUnlock(elem.second, false);
    }
}

} // namespace app
