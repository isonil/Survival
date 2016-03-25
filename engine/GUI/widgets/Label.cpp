#include "Label.hpp"

#include "../../util/AppTime.hpp"
#include "../../util/Color.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"
#include "../IGUITexture.hpp"

namespace engine
{
namespace GUI
{

Label::Label(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect},
      m_color{Color::k_white},
      m_hCentered{},
      m_vCentered{},
      m_useRichText{}
{
}

void Label::update(const AppTime &time, bool nonOccludedMouseOver)
{
}

void Label::draw() const
{
    if(m_useRichText)
        getRenderer().drawText(m_richText, getScreenRect().pos);
    else
        getRenderer().drawText(m_text, getScreenRect(), m_color,
                               IGUIRenderer::FontSize::Normal,
                               m_hCentered ? IGUIRenderer::HorizontalTextAlign::Center : IGUIRenderer::HorizontalTextAlign::Left,
                               m_vCentered ? IGUIRenderer::VerticalTextAlign::Middle : IGUIRenderer::VerticalTextAlign::Top);
}

bool Label::anyNonInternalChildWidgets() const
{
    return false;
}

void Label::setCentered(bool hCentered, bool vCentered)
{
    m_hCentered = hCentered;
    m_vCentered = vCentered;
}

void Label::setText(const std::string &text)
{
    m_text = text;
    m_useRichText = false;
}

void Label::setText(const RichText &text)
{
    m_richText = text;
    m_useRichText = true;
}

void Label::setColor(const Color &color)
{
    m_color = color;
}

} // namespace GUI
} // namespace engine
