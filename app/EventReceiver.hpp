#ifndef APP_EVENT_RECEIVER_HPP
#define APP_EVENT_RECEIVER_HPP

#include "engine/GUI/IEventReceiver.hpp"

namespace app
{

class EventReceiver : public engine::GUI::IEventReceiver
{
public:
    void onEvent(engine::GUI::Event &event) override;
};

} // namespace app

#endif // APP_EVENT_RECEIVER_HPP
