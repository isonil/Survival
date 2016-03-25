#ifndef APP_UNLOCKED_UPGRAES_HPP
#define APP_UNLOCKED_UPGRAES_HPP

#include <unordered_set>
#include <memory>

namespace app
{

class Character;
class UpgradeDef;
class PlayerComponent;

class UnlockedUpgrades
{
public:
    UnlockedUpgrades(Character &character, PlayerComponent &playerComponent);

    bool tryUnlock(const std::shared_ptr <UpgradeDef> &upgradeDef, bool removeUpgradePoints = true);
    bool isUnlocked(const std::shared_ptr <UpgradeDef> &upgradeDef) const;
    bool isUnlocked(const UpgradeDef &upgradeDef) const;
    int getUpgradePoints() const;
    void addUpgradePoints(int points);

private:
    void unlockUnlockedByDefault();

    Character &m_character;
    PlayerComponent &m_playerComponent;
    std::unordered_set <std::shared_ptr <UpgradeDef>> m_unlockedUpgradeDefs;
    int m_upgradePoints;
};

} // namespace app

#endif // APP_UNLOCKED_UPGRAES_HPP
