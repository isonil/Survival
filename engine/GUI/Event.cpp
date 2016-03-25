#include "Event.hpp"

namespace engine
{
namespace GUI
{

Event::Event()
    : m_type{Type::None},
      m_keyChar{},
      m_keyCode{},
      m_ctrl{},
      m_shift{},
      m_mouseButton{MouseButton::None},
      m_mouseAction{MouseAction::None},
      m_absorbed{}
{
}

Event::Event(Type type)
    : m_type{type},
      m_keyChar{},
      m_keyCode{},
      m_ctrl{},
      m_shift{},
      m_mouseButton{MouseButton::None},
      m_mouseAction{MouseAction::None},
      m_absorbed{}
{
}

Event::Event(wchar_t keyChar, irr::EKEY_CODE keyCode, bool ctrl, bool shift)
    : m_type{Type::KeyboardEvent},
      m_keyChar{keyChar},
      m_keyCode{keyCode},
      m_ctrl{ctrl},
      m_shift{shift},
      m_mouseButton{MouseButton::None},
      m_mouseAction{MouseAction::None},
      m_absorbed{}
{
}

Event::Event(MouseAction action, const IntVec2 &pos, bool ctrl, bool shift, MouseButton button)
    : m_type{Type::MouseEvent},
      m_keyChar{},
      m_keyCode{},
      m_ctrl{ctrl},
      m_shift{shift},
      m_mouseButton{button},
      m_mouseAction{action},
      m_pos{pos},
      m_absorbed{}
{
}

void Event::absorb()
{
    m_absorbed = true;
}

bool Event::isAbsorbed() const
{
    return m_absorbed;
}

bool Event::isSubstantial() const
{
    if(m_type == Event::Type::MouseEvent &&
       (m_mouseAction == Event::MouseAction::Down ||
        m_mouseAction == Event::MouseAction::Double ||
        m_mouseAction == Event::MouseAction::Triple ||
        m_mouseAction == Event::MouseAction::WheelUp ||
        m_mouseAction == Event::MouseAction::WheelDown)) {
        return true;
    }
    else if(m_type == Event::Type::GainedFocus ||
            m_type == Event::Type::LostFocus) {
        return true;
    }

    // keyboard key down event used to be substantial, but it was removed
    // so further event receivers could still use not-really-used keyboard events.
    // currently if one wants to absorb key down event, it must be done manually.

    return false;
}

Event::Type Event::getType() const
{
    return m_type;
}

wchar_t Event::getKeyChar() const
{
    return m_keyChar;
}

irr::EKEY_CODE Event::getKeyCode() const
{
    return m_keyCode;
}

bool Event::isCtrl() const
{
    return m_ctrl;
}

bool Event::isShift() const
{
    return m_shift;
}

Event::MouseButton Event::getMouseButton() const
{
    return m_mouseButton;
}

Event::MouseAction Event::getMouseAction() const
{
    return m_mouseAction;
}

const IntVec2 &Event::getPosition() const
{
    return m_pos;
}

} // namespace GUI
} // namespace engine
