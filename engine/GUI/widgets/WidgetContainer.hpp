#ifndef ENGINE_GUI_WIDGET_CONTAINER_HPP
#define ENGINE_GUI_WIDGET_CONTAINER_HPP

#include "../../util/Rect.hpp"
#include "Widget.hpp"

#include <memory>
#include <vector>

namespace engine { class AppTime; }

namespace engine
{
namespace GUI
{

class Event;
class IGUIRenderer;
class Button;
class EditBox;
class Window;
class Image;
class Label;
class RectWidget;
class ProgressBar;

class WidgetContainer : public Widget
{
public:
    WidgetContainer(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void onEvent(Event &event) override;
    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;
    void kill() override;

    void moveChildWidgetToBeAfter(const Widget &widget, const Widget &afterThisOne);
    void moveChildWidgetToBeBefore(const Widget &widget, const Widget &beforeThisOne);
    const Widget *getNonOccludedLocalWidgetAt(const IntVec2 &globalPos) const;

    std::shared_ptr <Button> addButton(const IntRect &rect);
    std::shared_ptr <EditBox> addEditBox(const IntRect &rect);
    std::shared_ptr <Window> addWindow(const IntRect &rect);
    std::shared_ptr <Image> addImage(const IntRect &rect);
    std::shared_ptr <Label> addLabel(const IntRect &rect);
    std::shared_ptr <RectWidget> addRectWidget(const IntRect &rect);
    std::shared_ptr <ProgressBar> addProgressBar(const IntRect &rect);

    template <typename T, typename... Args> std::shared_ptr <T> addCustomWidget(const IntRect &rect, Args&&... args);

protected:
    int getChildWidgetsCount() const;

private:
    using base = Widget;

    void moveChildWidget(const Widget &widget, const Widget &relativeToThisOne, bool toBeAfter);
    bool shouldRemoveThisWidget(const std::shared_ptr <Widget> &widget) const;

    Widget *m_focusedWidget;
    std::vector <std::shared_ptr <Widget>> m_widgets;
};

template <typename T, typename... Args> std::shared_ptr <T> WidgetContainer::addCustomWidget(const IntRect &rect, Args&&... args)
{
    TRACK;

    const auto &ptr = std::make_shared <T> (this, getRendererPtr(), rect, args...);
    m_widgets.push_back(ptr);
    return ptr;
}

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_WIDGET_CONTAINER_HPP
