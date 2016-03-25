#ifndef ENGINE_GUI_INPUT_EVENT_HPP
#define ENGINE_GUI_INPUT_EVENT_HPP

#include "../util/Vec2.hpp"

#include <irrlicht/Keycodes.h>

namespace engine
{
namespace GUI
{

class Event
{
public:
    enum class Type
    {
        None,
        KeyboardEvent,
        MouseEvent,
        GainedFocus,
        LostFocus
    };

    enum class MouseButton
    {
        None,
        Left,
        Right,
        Middle
    };

    enum class MouseAction
    {
        None,
        Move,
        Up,
        Down,
        Double,
        Triple,
        WheelUp,
        WheelDown
    };

    Event();
    Event(Type type); // universal constructor
    Event(wchar_t keyChar, irr::EKEY_CODE keyCode, bool ctrl, bool shift); // keyboard event
    Event(MouseAction action, const IntVec2 &pos, bool ctrl, bool shift, MouseButton = MouseButton::None); // mouse event

    void absorb();
    bool isAbsorbed() const;
    bool isSubstantial() const;
    Type getType() const;
    wchar_t getKeyChar() const;
    irr::EKEY_CODE getKeyCode() const;
    bool isCtrl() const;
    bool isShift() const;
    MouseButton getMouseButton() const;
    MouseAction getMouseAction() const;
    const IntVec2 &getPosition() const;

private:
    Type m_type;
    wchar_t m_keyChar;
    irr::EKEY_CODE m_keyCode;
    bool m_ctrl, m_shift;
    MouseButton m_mouseButton;
    MouseAction m_mouseAction;
    IntVec2 m_pos;
    bool m_absorbed;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_INPUT_EVENT_HPP
