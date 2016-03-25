#ifndef ENGINE_GUI_I_EVENT_RECEIVER_HPP
#define ENGINE_GUI_I_EVENT_RECEIVER_HPP

namespace engine
{
namespace GUI
{

class Event;

class IEventReceiver
{
public:
    virtual void onEvent(Event &event) = 0;

    virtual ~IEventReceiver() = default;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_I_EVENT_RECEIVER_HPP
