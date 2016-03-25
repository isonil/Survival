#ifndef ENGINE_GUI_WIDGET_HPP
#define ENGINE_GUI_WIDGET_HPP

#include "../../util/Trace.hpp"
#include "../../util/Rect.hpp"
#include "../IEventReceiver.hpp"

namespace engine { class AppTime; }

namespace engine
{
namespace GUI
{

class Event;
class IGUIRenderer;
class WidgetContainer;

class Widget : public IEventReceiver, public Tracked <Widget>
{
public:
    Widget(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void onEvent(Event &event) override;
    virtual void update(const AppTime &time, bool nonOccludedMouseOver) = 0;
    virtual void draw() const = 0;
    virtual bool moveToFrontOnFocus() const;
    virtual bool anyNonInternalChildWidgets() const = 0;
    virtual void kill();

    bool isDead() const;
    void setPosition(const IntVec2 &pos);
    void moveToBeBefore(const Widget &beforeThisOne);
    void moveToBeAfter(const Widget &afterThisOne);
    bool hasFocus() const;
    const IntRect &getRect() const;
    IntRect &getRect();
    IntRect getScreenRect() const;
    IGUIRenderer &getRenderer() const;

    void onRemovedParent();

protected:
    const std::shared_ptr <IGUIRenderer> &getRendererPtr() const;
    const IntVec2 &getMousePos() const;
    bool hasParent() const;
    const WidgetContainer &getParent() const;
    bool isNonOccludedAt(const engine::IntVec2 &pos) const;

private:
    WidgetContainer *m_parent;
    std::shared_ptr <IGUIRenderer> m_renderer;
    IntRect m_rect;
    IntVec2 m_mousePos;
    bool m_hasFocus;
    bool m_isDead;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_WIDGET_HPP
