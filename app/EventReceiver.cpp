#include "EventReceiver.hpp"

#include "engine/GUI/Event.hpp"
#include "GUI/immediate/HandyWindow.hpp"
#include "GUI/MainGUI.hpp"
#include "entities/Character.hpp"
#include "thisPlayer/ThisPlayer.hpp"
#include "Global.hpp"
#include "Core.hpp"

namespace app
{

void EventReceiver::onEvent(engine::GUI::Event &event)
{
    // if we are in GUI mode (events are first propgated to widgets),
    // and we ended up here, then it means that we are informed
    // about non-substantial event like mouse button up, or not
    // handled keyboard event

    // highest priority - toggle GUI mode.
    // if we are in GUI mode, then it means that no widget absorbed this keyboard event,
    // so we toggle GUI mode

    auto &core = Global::getCore();
    auto &mainGUI = core.getMainGUI();

    if(event.getType() == engine::GUI::Event::Type::KeyboardEvent && event.getKeyCode() == irr::KEY_KEY_C) {
        mainGUI.toggleGUIMode();
        return;
    }

    // if we are in GUI mode, then we absorb all keyboard events here - we don't want
    // to control player when in GUI mode, even if some keyboard events were not handled anywhere
    // note: there are no keyboard key Up events, so KeyboardEvent means key Down,
    //       otherwise we would return here only for Down events
    if(mainGUI.isInGUIMode() && event.getType() == engine::GUI::Event::Type::KeyboardEvent)
        return;

    // propagate event to current handy window, if there is one
    if(mainGUI.isAnyCurrentHandyWindow())
        mainGUI.getCurrentHandyWindow().onEvent(event);

    // we don't care if handy window absorbed this event, or not, because we
    // want to be able to control player no matter what handy window did

    // toggle handy windows
    if(event.getType() == engine::GUI::Event::Type::KeyboardEvent) {
        switch(event.getKeyCode()) {
        case irr::KEY_KEY_1:
            mainGUI.toggleHandyWindow(HandyWindow::Type::Construct);
            break;

        case irr::KEY_KEY_2:
            mainGUI.toggleHandyWindow(HandyWindow::Type::SkillsAndUpgrades);
            break;

        case irr::KEY_KEY_3:
            mainGUI.toggleHandyWindow(HandyWindow::Type::Inventory);
            break;

        default:
            break;
        }
    }

    // finally, propagate event to Player
    core.getThisPlayer().onEvent(event);
}

} // namespace app
