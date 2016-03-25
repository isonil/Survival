#include "Window.hpp"

#include "../../util/AppTime.hpp"
#include "../../util/Color.hpp"
#include "../../util/Math.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"
#include "Button.hpp"

namespace engine
{
namespace GUI
{

Window::Window(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : WidgetContainer{parent, renderer, rect},
      m_mouseOverAccumulator{},
      m_dragging{}
{
    m_closeButton = addButton(IntRect{rect.size.x - 21, 7, 12, 12});
    m_closeButton->setTexture(renderer->getTexture(k_closeButtonTexturePath));
    m_closeButton->setDrawBackground(false);
    m_closeButton->setOnPressed([this]() { this->kill(); });
}

void Window::onEvent(Event &event)
{
    const auto &type = event.getType();
    const auto &mouseAction = event.getMouseAction();
    const auto &mouseButton = event.getMouseButton();

    if(type == Event::Type::MouseEvent) {
        if(m_dragging)
            getRect().pos = event.getPosition() - m_draggingOffset - getScreenRect().pos + getRect().pos;

        if(mouseButton == Event::MouseButton::Left) {
            if(mouseAction == Event::MouseAction::Down) {
                const auto &titleBar = getTitleBarScreenRect();

                if(titleBar.contains(event.getPosition()) && !m_closeButton->getScreenRect().contains(event.getPosition())) {
                    m_dragging = true;
                    m_draggingOffset = event.getPosition() - getScreenRect().pos;
                }
            }
            else if(mouseAction == Event::MouseAction::Up)
                m_dragging = false;
        }
    }

    WidgetContainer::onEvent(event);
}

void Window::update(const AppTime &time, bool nonOccludedMouseOver)
{
    WidgetContainer::update(time, nonOccludedMouseOver);

    if(nonOccludedMouseOver)
        m_mouseOverAccumulator += time.getDelta() * k_mouseOverAccumulatorFactor;
    else
        m_mouseOverAccumulator -= time.getDelta() * k_mouseOverAccumulatorFactor;

    m_mouseOverAccumulator = Math::clamp01(m_mouseOverAccumulator);
}

void Window::draw() const
{
    static const Color titleBarGray{0.137f, 0.137f, 0.137f};
    static const Color titleBarBlack{0.f, 0.f, 0.f};
    static const Color contentsBackgroundTop{0.175f, 0.175f, 0.175f};
    static const Color contentsBackgroundBottom{0.143f, 0.143f, 0.143f};
    static const Color grayTopLine{0.2f, 0.2f, 0.2f};
    static const Color border{0.078f, 0.078f, 0.078f};
    static const Color shadow{0.f, 0.f, 0.f, 0.35f};

    const auto &screenRect = getScreenRect();
    const auto &titleBarRect = getTitleBarScreenRect();
    const auto &contentsRect = screenRect.movedTopBorder(titleBarRect.size.y);
    const auto &outerShadowRect = screenRect.getOuterRect(15.f);
    const auto &renderer = getRenderer();

    renderer.drawShadow(outerShadowRect, shadow);
    renderer.drawGradientRect(titleBarRect, titleBarGray, titleBarGray, titleBarBlack, titleBarBlack);
    renderer.drawGradientRect(contentsRect, contentsBackgroundTop, contentsBackgroundTop, contentsBackgroundBottom, contentsBackgroundBottom);
    renderer.drawRectOutline(screenRect, border);

    renderer.drawText(m_title, titleBarRect.pos.moved(10, 5), Color::k_white);

    const auto &topLineFrom = screenRect.pos.movedY(1);
    const auto &topLineTo = screenRect.pos.moved(screenRect.size.x, 1);

    renderer.drawLine(topLineFrom, topLineTo, grayTopLine);

    WidgetContainer::draw();
}

bool Window::moveToFrontOnFocus() const
{
    return true;
}

bool Window::anyNonInternalChildWidgets() const
{
    // we have closeButton as internal widget

    // make sure we really have it
    E_DASSERT(m_closeButton, "Close button is nullptr.");

    return getChildWidgetsCount() > 1;
}

void Window::setTitle(const std::string &title)
{
    m_title = title;
}

IntRect Window::getTitleBarScreenRect() const
{
    auto rect = getScreenRect();
    rect.size.y = 25;
    return rect;
}

const std::string Window::k_closeButtonTexturePath = "GUI/closeButton.png";
const float Window::k_mouseOverAccumulatorFactor{0.0025f};

} // namespace GUI
} // namespace engine
