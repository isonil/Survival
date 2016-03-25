#include "EventManager.hpp"

#include "../../GUI/Event.hpp"
#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"

namespace engine
{
namespace app3D
{

EventManager::EventManager()
    : m_isKeyPressed{}
{
}

void EventManager::registerEventReceiver(const std::shared_ptr <GUI::IEventReceiver> &eventReceiver)
{
    if(!eventReceiver)
        throw Exception{"Tried to register nullptr event receiver."};

    if(isEventReceiverRegistered(*eventReceiver))
        return;

    m_eventReceivers.push_back(eventReceiver);
}

void EventManager::registerEventReceiverAtTheBeginning(const std::shared_ptr <GUI::IEventReceiver> &eventReceiver)
{
    if(!eventReceiver)
        throw Exception{"Tried to register nullptr event receiver."};

    if(isEventReceiverRegistered(*eventReceiver))
        return;

    m_eventReceivers.insert(m_eventReceivers.begin(), eventReceiver);
}

void EventManager::deregisterEventReceiver(GUI::IEventReceiver &eventReceiver)
{
    for(size_t i = 0; i < m_eventReceivers.size(); ++i) {
        if(m_eventReceivers[i].get() == &eventReceiver) {
            m_eventReceivers.erase(m_eventReceivers.begin() + i);
            return;
        }
    }
}

bool EventManager::isKeyPressed(irr::EKEY_CODE key) const
{
    TRACK;

    return m_isKeyPressed[static_cast <size_t> (key)];
}

bool EventManager::isEventReceiverRegistered(GUI::IEventReceiver &eventReceiver) const
{
    return std::any_of(m_eventReceivers.begin(), m_eventReceivers.end(), [&eventReceiver](const auto &elem) {
        return elem.get() == &eventReceiver;
    });
}

bool EventManager::OnEvent(const irr::SEvent &event)
{
    TRACK;

    if(event.EventType == irr::EET_LOG_TEXT_EVENT) {
        if(event.LogEvent.Level == irr::ELL_ERROR) {
            E_LOG_TAG("irrlicht error", "%s", event.LogEvent.Text);
        }
        else if(event.LogEvent.Level == irr::ELL_WARNING) {
            E_LOG_TAG("irrlicht warning", "%s", event.LogEvent.Text);
        }
        else {
            E_LOG_TAG("irrlicht info", "%s", event.LogEvent.Text);
        }

        return true;
    }
    else if(event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if(event.KeyInput.PressedDown)
            m_isKeyPressed[static_cast <size_t> (event.KeyInput.Key)] = true;
        else
            m_isKeyPressed[static_cast <size_t> (event.KeyInput.Key)] = false;
    }

    GUI::Event inputEvent;

    if(event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if(event.KeyInput.PressedDown)
            inputEvent = GUI::Event{event.KeyInput.Char, event.KeyInput.Key, event.KeyInput.Control, event.KeyInput.Shift};
    }
    else if(event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        bool ctrl{event.MouseInput.Control};
        bool shift{event.MouseInput.Shift};

        IntVec2 pos{event.MouseInput.X, event.MouseInput.Y};

        switch(event.MouseInput.Event) {
        case irr::EMIE_MOUSE_WHEEL:
            inputEvent = GUI::Event{event.MouseInput.Wheel > 0.f ?
                                    GUI::Event::MouseAction::WheelUp :
                                    GUI::Event::MouseAction::WheelDown,
                                    pos, ctrl, shift};
            break;
        case irr::EMIE_MOUSE_MOVED:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Move, pos, ctrl, shift};
            break;
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Down, pos, ctrl, shift, GUI::Event::MouseButton::Left};
            break;
        case irr::EMIE_RMOUSE_PRESSED_DOWN:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Down, pos, ctrl, shift, GUI::Event::MouseButton::Right};
            break;
        case irr::EMIE_MMOUSE_PRESSED_DOWN:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Down, pos, ctrl, shift, GUI::Event::MouseButton::Middle};
            break;
        case irr::EMIE_LMOUSE_LEFT_UP:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Up, pos, ctrl, shift, GUI::Event::MouseButton::Left};
            break;
        case irr::EMIE_RMOUSE_LEFT_UP:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Up, pos, ctrl, shift, GUI::Event::MouseButton::Right};
            break;
        case irr::EMIE_MMOUSE_LEFT_UP:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Up, pos, ctrl, shift, GUI::Event::MouseButton::Middle};
            break;
        case irr::EMIE_LMOUSE_DOUBLE_CLICK:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Double, pos, ctrl, shift, GUI::Event::MouseButton::Left};
            break;
        case irr::EMIE_RMOUSE_DOUBLE_CLICK:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Double, pos, ctrl, shift, GUI::Event::MouseButton::Right};
            break;
        case irr::EMIE_MMOUSE_DOUBLE_CLICK:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Double, pos, ctrl, shift, GUI::Event::MouseButton::Middle};
            break;
        case irr::EMIE_LMOUSE_TRIPLE_CLICK:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Triple, pos, ctrl, shift, GUI::Event::MouseButton::Left};
            break;
        case irr::EMIE_RMOUSE_TRIPLE_CLICK:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Triple, pos, ctrl, shift, GUI::Event::MouseButton::Right};
            break;
        case irr::EMIE_MMOUSE_TRIPLE_CLICK:
            inputEvent = GUI::Event{GUI::Event::MouseAction::Triple, pos, ctrl, shift, GUI::Event::MouseButton::Middle};
            break;
        default:
            break;
        }
    }

    if(inputEvent.getType() != GUI::Event::Type::None && !inputEvent.isAbsorbed()) {
        // we make a local copy, in case event receivers container is modified during onEvent
        m_eventReceivers_work = m_eventReceivers;

        for(auto &elem : m_eventReceivers_work) {
            E_DASSERT(elem, "Event receiver is nullptr.");

            elem->onEvent(inputEvent);

            if(inputEvent.isAbsorbed()) {
                m_eventReceivers_work.clear();
                return true;
            }
        }

        m_eventReceivers_work.clear();
    }

    return false;
}

} // namespace app3D
} // namespace engine
