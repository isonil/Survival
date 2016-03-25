#include "CharacterComponent.hpp"

namespace app
{

CharacterComponent::CharacterComponent(Character &character)
    : m_character{character}
{
}

Character &CharacterComponent::getCharacter()
{
    return m_character;
}

const Character &CharacterComponent::getCharacter() const
{
    return m_character;
}

} // namespace app
