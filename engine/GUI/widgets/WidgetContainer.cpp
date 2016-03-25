#include "WidgetContainer.hpp"

#include "../../util/AppTime.hpp"
#include "../../util/LogManager.hpp"
#include "../Event.hpp"
#include "Button.hpp"
#include "EditBox.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "Label.hpp"
#include "RectWidget.hpp"
#include "ProgressBar.hpp"

namespace engine
{
namespace GUI
{

WidgetContainer::WidgetContainer(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect},
      m_focusedWidget{}
{
}

void WidgetContainer::onEvent(Event &event)
{
    TRACK;

    Widget::onEvent(event);

    const auto &type = event.getType();
    const auto &mouseAction = event.getMouseAction();

    // lost focus event
    if(type == Event::Type::LostFocus) {
        if(m_focusedWidget) {
            m_focusedWidget->onEvent(event);
            m_focusedWidget = nullptr;
            return;
        }
    }

    // gained focus event
    if(type == Event::Type::GainedFocus)
        return;

    // resolving new focused child widget
    if(type == Event::Type::MouseEvent && mouseAction == Event::MouseAction::Down) {
        int moveToFront{-1};

        for(int i = static_cast <int> (m_widgets.size()) - 1; i >= 0; --i) {
            E_DASSERT(m_widgets[i], "Widget is nullptr.");

            if(m_widgets[i]->getScreenRect().contains(event.getPosition())) {
                if(m_focusedWidget == m_widgets[i].get()) {
                    if(m_widgets[i]->moveToFrontOnFocus())
                        moveToFront = i;

                    break;
                }

                if(m_focusedWidget) {
                    Event lostFocus{Event::Type::LostFocus};
                    m_focusedWidget->onEvent(lostFocus);
                }

                m_focusedWidget = m_widgets[i].get();

                Event gainedFocus{Event::Type::GainedFocus};
                m_focusedWidget->onEvent(gainedFocus);

                if(m_focusedWidget->moveToFrontOnFocus())
                    moveToFront = i;

                break;
            }
        }

        if(moveToFront >= 0) {
            auto widget = m_widgets[moveToFront];
            m_widgets.erase(m_widgets.begin() + moveToFront);
            m_widgets.push_back(widget);
        }
    }

    // send event to focused widget first
    if(m_focusedWidget) {
        m_focusedWidget->onEvent(event);
        if(event.isAbsorbed())
            return;
    }

    // send event to all widgets until event is absorbed
    for(int i = static_cast <int> (m_widgets.size()) - 1; i >= 0; --i) {
        E_DASSERT(m_widgets[i], "Widget is nullptr.");

        if(m_widgets[i].get() == m_focusedWidget)
            continue;

        m_widgets[i]->onEvent(event);

        if(event.isAbsorbed())
            return;
    }
}

void WidgetContainer::update(const AppTime &time, bool nonOccludedMouseOver)
{
    TRACK;

    bool mouseOverAbsorbed{!nonOccludedMouseOver};

    for(int i = static_cast <int> (m_widgets.size()) - 1; i >= 0; --i) {
        E_DASSERT(m_widgets[i], "Widget is nullptr.");

        if(shouldRemoveThisWidget(m_widgets[i])) {
            if(m_focusedWidget == m_widgets[i].get())
                m_focusedWidget = nullptr;

            m_widgets[i]->onRemovedParent();
            m_widgets.erase(m_widgets.begin() + i);
        }
        else {
            bool childMouseOver{};

            if(!mouseOverAbsorbed && m_widgets[i]->getScreenRect().contains(getMousePos())) {
                mouseOverAbsorbed = true;
                childMouseOver = true;
            }

            m_widgets[i]->update(time, childMouseOver);
        }
    }
}

void WidgetContainer::draw() const
{
    TRACK;

    for(const auto &elem : m_widgets) {
        E_DASSERT(elem, "Widget is nullptr.");
        elem->draw();
    }
}

bool WidgetContainer::anyNonInternalChildWidgets() const
{
    return !m_widgets.empty();
}

void WidgetContainer::kill()
{
    for(auto &elem : m_widgets) {
        E_DASSERT(elem, "Widget is nullptr.");
        elem->kill();
    }

    base::kill();
}

void WidgetContainer::moveChildWidgetToBeAfter(const Widget &widget, const Widget &afterThisOne)
{
    TRACK;

    moveChildWidget(widget, afterThisOne, true);
}

void WidgetContainer::moveChildWidgetToBeBefore(const Widget &widget, const Widget &beforeThisOne)
{
    TRACK;

    moveChildWidget(widget, beforeThisOne, false);
}

const Widget *WidgetContainer::getNonOccludedLocalWidgetAt(const IntVec2 &globalPos) const
{
    if(hasParent() && getParent().getNonOccludedLocalWidgetAt(globalPos) != this)
        return nullptr;

    for(int i = static_cast <int> (m_widgets.size()) - 1; i >= 0; --i) {
        E_DASSERT(m_widgets[i], "Widget is nullptr.");

        if(m_widgets[i]->getScreenRect().contains(globalPos))
            return m_widgets[i].get();
    }

    if(getScreenRect().contains(globalPos))
        return this;

    return nullptr;
}

std::shared_ptr <Button> WidgetContainer::addButton(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <Button> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

std::shared_ptr <EditBox> WidgetContainer::addEditBox(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <EditBox> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

std::shared_ptr <Window> WidgetContainer::addWindow(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <Window> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

std::shared_ptr <Image> WidgetContainer::addImage(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <Image> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

std::shared_ptr <Label> WidgetContainer::addLabel(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <Label> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

std::shared_ptr <RectWidget> WidgetContainer::addRectWidget(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <RectWidget> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

std::shared_ptr <ProgressBar> WidgetContainer::addProgressBar(const IntRect &rect)
{
    TRACK;

    const auto &ptr = std::make_shared <ProgressBar> (this, getRendererPtr(), rect);
    m_widgets.push_back(ptr);
    return ptr;
}

int WidgetContainer::getChildWidgetsCount() const
{
    return m_widgets.size();
}

void WidgetContainer::moveChildWidget(const Widget &widget, const Widget &relativeToThisOne, bool toBeAfter)
{
    TRACK;

    auto widgetToMove_iter = std::find_if(m_widgets.begin(), m_widgets.end(), [&widget](const auto &elem) {
        E_DASSERT(elem, "Widget is nullptr.");
        return elem.get() == &widget;
    });

    if(widgetToMove_iter == m_widgets.end())
        return;

    auto relativeToThisOne_iter = std::find_if(m_widgets.begin(), m_widgets.end(), [&relativeToThisOne](const auto &elem) {
        E_DASSERT(elem, "Widget is nullptr.");
        return elem.get() == &relativeToThisOne;
    });

    if(relativeToThisOne_iter == m_widgets.end())
        return;

    auto movedWidget = *widgetToMove_iter;
    m_widgets.erase(relativeToThisOne_iter);

    relativeToThisOne_iter = std::find_if(m_widgets.begin(), m_widgets.end(), [&relativeToThisOne](const auto &elem) {
        E_DASSERT(elem, "Widget is nullptr.");
        return elem.get() == &relativeToThisOne;
    });

    E_DASSERT(relativeToThisOne_iter != m_widgets.end(), "Could not find widget (could find it before).");

    if(toBeAfter)
        ++relativeToThisOne_iter;

    m_widgets.insert(relativeToThisOne_iter, movedWidget);
}

bool WidgetContainer::shouldRemoveThisWidget(const std::shared_ptr<Widget> &widget) const
{
    if(!widget)
        return true;

    if(widget->isDead())
        return true;

    return widget.unique() && !widget->anyNonInternalChildWidgets();
}

} // namespace GUI
} // namespace engine
