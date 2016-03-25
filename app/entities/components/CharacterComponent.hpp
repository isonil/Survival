#ifndef APP_CHARACTER_COMPONENT_HPP
#define APP_CHARACTER_COMPONENT_HPP

namespace app
{

class Character;

class CharacterComponent
{
public:
    CharacterComponent(Character &character);

    virtual void onInWorldUpdate() = 0;

    virtual ~CharacterComponent() = default;

protected:
    Character &getCharacter();
    const Character &getCharacter() const;

private:
    Character &m_character;
};

} // namespace app

#endif // APP_CHARACTER_COMPONENT_HPP
