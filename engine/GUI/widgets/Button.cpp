#include "Button.hpp"

#include "../../app3D/defs/SoundDef.hpp"
#include "../../util/AppTime.hpp"
#include "../../util/Color.hpp"
#include "../../util/DefDatabase.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"
#include "../IGUITexture.hpp"

namespace engine
{
namespace GUI
{

Button::Button(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect},
      m_clickSound{renderer->getDefDatabase().getDef <app3D::SoundDef> (k_clickSoundDefName)},
      m_mouseOverAccumulator{},
      m_pressed{},
      m_drawBackground{true}
{
}

void Button::onEvent(Event &event)
{
    Widget::onEvent(event);

    const auto &type = event.getType();
    const auto &mouseButton = event.getMouseButton();
    const auto &mouseAction = event.getMouseAction();

    if(type == Event::Type::MouseEvent && mouseButton == Event::MouseButton::Left) {
        if(mouseAction == Event::MouseAction::Down && getScreenRect().contains(event.getPosition()))
            m_pressed = true;

        if(mouseAction == Event::MouseAction::Up) {
            if(m_pressed) {
                m_pressed = false;

                if(getScreenRect().contains(event.getPosition()) && isNonOccludedAt(event.getPosition())) {
                    m_clickSound.play();

                    if(m_onPressed)
                        m_onPressed();
                }
            }
        }
    }
}

void Button::update(const AppTime &time, bool nonOccludedMouseOver)
{
    if(nonOccludedMouseOver)
        m_mouseOverAccumulator += time.getDelta() * k_mouseOverAccumulatorFactor;
    else
        m_mouseOverAccumulator -= time.getDelta() * k_mouseOverAccumulatorFactor;

    m_mouseOverAccumulator = Math::clamp01(m_mouseOverAccumulator);
}

void Button::draw() const
{
    const auto &screenRect = getScreenRect();
    const auto &renderer = getRenderer();

    if(m_drawBackground) {
        static const Color border{Color::k_black};
        static const Color topLine{51, 51, 51};
        static const Color upLight{62, 116, 189};
        static const Color backgroundLight{25, 25, 25};
        static const Color backgroundDark{20, 20, 20};

        const auto &backgroundWithUpLight = backgroundLight.mixed(upLight, m_mouseOverAccumulator / 6.f);

        if(m_pressed) {
            renderer.drawGradientRect(screenRect,
                backgroundWithUpLight.mixed(backgroundLight, 0.5f),
                backgroundWithUpLight.mixed(backgroundLight, 0.5f),
                backgroundDark,
                backgroundDark);
        }
        else {
            renderer.drawGradientRect(screenRect,
                backgroundWithUpLight,
                backgroundWithUpLight,
                backgroundDark,
                backgroundDark);
        }

        renderer.drawRectOutline(screenRect, border);

        const auto &lineFrom = screenRect.pos.moved(1, 1);
        const IntVec2 lineTo{screenRect.getMaxX() - 1, lineFrom.y};

        if(!m_pressed)
            renderer.drawLine(lineFrom, lineTo, topLine.mixed(upLight, m_mouseOverAccumulator));

        if(m_texture) {
            auto textureRect = screenRect.getInnerSquare();

            if(m_pressed)
                textureRect.move(1, 1);

            m_texture->draw(textureRect, m_pressed ? Color{200, 200, 200} : Color::k_white);
        }
    }
    else if(m_texture)
        m_texture->draw(screenRect, m_pressed ? Color{200, 200, 200} : Color::k_white);

    auto textRect = screenRect;

    if(m_pressed)
        textRect.move(1, 1);

    renderer.drawText(m_text, textRect, Color::k_white, IGUIRenderer::FontSize::Normal, IGUIRenderer::HorizontalTextAlign::Center, IGUIRenderer::VerticalTextAlign::Middle);
}

bool Button::anyNonInternalChildWidgets() const
{
    return false;
}

void Button::setTexture(const std::shared_ptr <IGUITexture> &texture)
{
    m_texture = texture;
}

void Button::setText(const std::string &text)
{
    m_text = text;
}

void Button::setDrawBackground(bool val)
{
    m_drawBackground = val;
}

void Button::setOnPressed(std::function <void()> onPressed)
{
    m_onPressed = onPressed;
}

const float Button::k_mouseOverAccumulatorFactor{0.0025f};
const std::string Button::k_clickSoundDefName = "Sound_ButtonClick";

} // namespace GUI
} // namespace engine
