#include "Image.hpp"

#include "../../util/AppTime.hpp"
#include "../../util/Color.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"
#include "../IGUITexture.hpp"

namespace engine
{
namespace GUI
{

Image::Image(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect},
      m_color{Color::k_white}
{
}

void Image::update(const AppTime &time, bool nonOccludedMouseOver)
{
    if(nonOccludedMouseOver && m_onMouseOver)
        m_onMouseOver();
}

void Image::draw() const
{
    if(m_texture)
        m_texture->draw(getScreenRect(), m_color);
    else
        getRenderer().drawText("[image]", getScreenRect(), m_color, IGUIRenderer::FontSize::Normal, IGUIRenderer::HorizontalTextAlign::Center, IGUIRenderer::VerticalTextAlign::Middle);
}

bool Image::anyNonInternalChildWidgets() const
{
    return false;
}

void Image::setOnMouseOver(std::function <void()> onMouseOver)
{
    m_onMouseOver = onMouseOver;
}

void Image::setTexture(const std::shared_ptr <IGUITexture> &texture)
{
    m_texture = texture;
}

void Image::setColor(const Color &color)
{
    m_color = color;
}

} // namespace GUI
} // namespace engine
