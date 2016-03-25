#include "EditBox.hpp"

#include "../../util/Color.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"

namespace engine
{
namespace GUI
{

EditBox::EditBox(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect},
      m_enteringText{},
      m_selectingText{}
{
}

void EditBox::onEvent(Event &event)
{
    Widget::onEvent(event);

    const auto &type = event.getType();
    const auto &mouseButton = event.getMouseButton();
    const auto &mouseAction = event.getMouseAction();

    if(type == Event::Type::MouseEvent && mouseButton == Event::MouseButton::Left) {
        if(mouseAction == Event::MouseAction::Down) {
            if(getScreenRect().contains(event.getPosition())) {
                m_enteringText = true;
                m_selectingText = true;
                m_selectedText = IntRange{};
            }
            else {
                m_enteringText = false;
                m_selectingText = false;
                m_selectedText = IntRange{};
            }
        }

        if(mouseAction == Event::MouseAction::Up)
            m_selectingText = false;
    }

    if(type == Event::Type::KeyboardEvent) {
        if(m_enteringText) {
            if(event.getKeyChar())
                m_text += event.getKeyChar();

            event.absorb();
        }
    }

    if(type == Event::Type::LostFocus) {
        m_enteringText = false;
        m_selectingText = false;
    }
}

void EditBox::update(const AppTime &time, bool nonOccludedMouseOver)
{
}

void EditBox::draw() const
{
    static const Color border{65, 65, 65};
    static const Color background{48, 48, 48};
    static const Color extraBorder{0, 0, 0};
    static const Color light{62, 116, 189};

    const auto &screenRect = getScreenRect();
    const auto &renderer = getRenderer();

    renderer.drawFilledRect(screenRect, background);
    renderer.drawRectOutline(screenRect.movedTopBorder(1).movedLeftBorder(1), border);

    auto lineFrom = screenRect.pos;
    auto lineTo = screenRect.pos.movedX(screenRect.size.x);

    renderer.drawLine(lineFrom, lineTo, extraBorder);

    lineTo = screenRect.pos.movedY(screenRect.size.y);

    renderer.drawLine(lineFrom, lineTo, extraBorder);

    if(m_enteringText) {
        // FIXME: temporary solution to check how it would look, TODO

        float m_enteringTextAccumulator{1.f};

        lineFrom = screenRect.pos.moved(1, 1);
        lineTo = screenRect.pos.moved(1, screenRect.size.y - 1);

        renderer.drawLine(lineFrom, lineTo, background.mixed(light, m_enteringTextAccumulator));

        lineFrom = screenRect.pos.moved(screenRect.size.x - 1, 1);
        lineTo = screenRect.pos.moved(screenRect.size.x - 1, screenRect.size.y - 1);

        renderer.drawLine(lineFrom, lineTo, background.mixed(light, m_enteringTextAccumulator));
    }

    const auto &textRect = screenRect.movedLeftBorder(4).movedRightBorder(-4);

    renderer.drawText(m_text, textRect, Color::k_white, IGUIRenderer::FontSize::Normal, IGUIRenderer::HorizontalTextAlign::Left, IGUIRenderer::VerticalTextAlign::Middle);
}

bool EditBox::anyNonInternalChildWidgets() const
{
    return false;
}

} // namespace GUI
} // namespace engine
