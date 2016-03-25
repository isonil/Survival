#include "PlayerComponent.hpp"

namespace app
{

PlayerComponent::PlayerComponent(Character &character)
    : CharacterComponent{character},
      m_unlockedUpgrades{character, *this},
      m_skills{character, *this}
{
}

void PlayerComponent::onInWorldUpdate()
{
}

UnlockedStructures &PlayerComponent::getUnlockedStructures()
{
    return m_unlockedStructures;
}

const UnlockedStructures &PlayerComponent::getUnlockedStructures() const
{
    return m_unlockedStructures;
}

Skills &PlayerComponent::getSkills()
{
    return m_skills;
}

const Skills &PlayerComponent::getSkills() const
{
    return m_skills;
}

UnlockedUpgrades &PlayerComponent::getUnlockedUpgrades()
{
    return m_unlockedUpgrades;
}

const UnlockedUpgrades &PlayerComponent::getUnlockedUpgrades() const
{
    return m_unlockedUpgrades;
}

} // namespace app
