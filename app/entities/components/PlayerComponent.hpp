#ifndef APP_PLAYER_COMPONENT_HPP
#define APP_PLAYER_COMPONENT_HPP

#include "player/UnlockedStructures.hpp"
#include "player/UnlockedUpgrades.hpp"
#include "player/Skills.hpp"
#include "CharacterComponent.hpp"

namespace app
{

class PlayerComponent : public CharacterComponent
{
public:
    PlayerComponent(Character &character);

    void onInWorldUpdate() override;

    UnlockedStructures &getUnlockedStructures();
    const UnlockedStructures &getUnlockedStructures() const;

    Skills &getSkills();
    const Skills &getSkills() const;

    UnlockedUpgrades &getUnlockedUpgrades();
    const UnlockedUpgrades &getUnlockedUpgrades() const;

private:
    UnlockedStructures m_unlockedStructures;
    UnlockedUpgrades m_unlockedUpgrades;
    Skills m_skills;
};

} // namespace app

#endif // APP_PLAYER_COMPONENT_HPP
