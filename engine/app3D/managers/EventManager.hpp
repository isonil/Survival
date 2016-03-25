#ifndef ENGINE_APP_3D_EVENT_MANAGER_HPP
#define ENGINE_APP_3D_EVENT_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../GUI/IEventReceiver.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <vector>

namespace engine
{
namespace app3D
{

class EventManager : public irr::IEventReceiver, public Tracked <EventManager>
{
public:
    EventManager();

    void registerEventReceiver(const std::shared_ptr <GUI::IEventReceiver> &eventReceiver);
    void registerEventReceiverAtTheBeginning(const std::shared_ptr <GUI::IEventReceiver> &eventReceiver);
    void deregisterEventReceiver(GUI::IEventReceiver &eventReceiver);
    bool isKeyPressed(irr::EKEY_CODE key) const;

private:
    bool isEventReceiverRegistered(GUI::IEventReceiver &eventReceiver) const;
    bool OnEvent(const irr::SEvent &event) override;

    std::vector <std::shared_ptr <GUI::IEventReceiver>> m_eventReceivers;
    std::vector <std::shared_ptr <GUI::IEventReceiver>> m_eventReceivers_work;
    bool m_isKeyPressed[irr::KEY_KEY_CODES_COUNT];
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_EVENT_MANAGER_HPP
