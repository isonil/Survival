#include "GUIManager.hpp"

#include "../util/AppTime.hpp"
#include "../util/Exception.hpp"
#include "IGUIRenderer.hpp"

namespace engine
{
namespace GUI
{

GUIManager::GUIManager(const std::shared_ptr <IGUIRenderer> &renderer)
    : WidgetContainer{nullptr, renderer, IntRect{}}
{
}

void GUIManager::onEvent(Event &event)
{
    TRACK;

    WidgetContainer::onEvent(event);
}

void GUIManager::draw() const
{
    TRACK;

    WidgetContainer::draw();
}

void GUIManager::update(const AppTime &time)
{
    TRACK;

    WidgetContainer::update(time, true);
}

void GUIManager::update(const AppTime &time, bool nonOccludedMouseOver)
{
    TRACK;

    WidgetContainer::update(time, nonOccludedMouseOver);
}

} // namespace GUI
} // namespace engine
