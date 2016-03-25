#include "Widget.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"
#include "WidgetContainer.hpp"

namespace engine
{
namespace GUI
{

Widget::Widget(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : m_parent{parent},
      m_renderer{renderer},
      m_rect{rect},
      m_hasFocus{},
      m_isDead{}
{
    if(!m_renderer)
        throw Exception{"Renderer is nullptr."};
}

void Widget::onEvent(Event &event)
{
    const auto &type = event.getType();
    const auto &mouseAction = event.getMouseAction();

    if(type == Event::Type::MouseEvent && mouseAction == Event::MouseAction::Move)
        m_mousePos = event.getPosition();

    if(type == Event::Type::GainedFocus)
        m_hasFocus = true;
    else if(type == Event::Type::LostFocus)
        m_hasFocus = false;

    if(event.isSubstantial())
        event.absorb();
}

bool Widget::moveToFrontOnFocus() const
{
    return false;
}

void Widget::kill()
{
    m_isDead = true;
}

bool Widget::isDead() const
{
    return m_isDead;
}

void Widget::setPosition(const IntVec2 &pos)
{
    m_rect.pos = pos;
}

void Widget::moveToBeBefore(const Widget &beforeThisOne)
{
    if(m_parent)
        m_parent->moveChildWidgetToBeBefore(*this, beforeThisOne);
}

void Widget::moveToBeAfter(const Widget &afterThisOne)
{
    if(m_parent)
        m_parent->moveChildWidgetToBeAfter(*this, afterThisOne);
}

bool Widget::hasFocus() const
{
    return m_hasFocus;
}

const IntRect &Widget::getRect() const
{
    return m_rect;
}

IntRect &Widget::getRect()
{
    return m_rect;
}

IntRect Widget::getScreenRect() const
{
    if(m_parent)
        return IntRect{m_parent->getScreenRect().pos + m_rect.pos, m_rect.size};

    return m_rect;
}

IGUIRenderer &Widget::getRenderer() const
{
    if(!m_renderer)
        throw Exception{"Renderer is nullptr."};

    return *m_renderer;
}

void Widget::onRemovedParent()
{
    m_parent = nullptr;
}

const std::shared_ptr <IGUIRenderer> &Widget::getRendererPtr() const
{
    return m_renderer;
}

const IntVec2 &Widget::getMousePos() const
{
    return m_mousePos;
}

bool Widget::hasParent() const
{
    return m_parent;
}

const WidgetContainer &Widget::getParent() const
{
    if(!m_parent)
        throw Exception{"Parent is nullptr."};

    return *m_parent;
}

bool Widget::isNonOccludedAt(const engine::IntVec2 &pos) const
{
    if(hasParent() && getParent().getNonOccludedLocalWidgetAt(pos) != this)
        return false;

    return true;
}

} // namespace GUI
} // namespace engine
