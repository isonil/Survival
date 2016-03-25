#include "RectWidget.hpp"

#include "../../util/AppTime.hpp"
#include "../../util/Color.hpp"
#include "../Event.hpp"
#include "../IGUIRenderer.hpp"
#include "../IGUITexture.hpp"

namespace engine
{
namespace GUI
{

RectWidget::RectWidget(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect}
{
}

void RectWidget::update(const AppTime &time, bool nonOccludedMouseOver)
{
}

void RectWidget::draw() const
{
    getRenderer().drawGradientRect(getScreenRect(), m_color_topLeft, m_color_topRight, m_color_botRight, m_color_botLeft);
}

bool RectWidget::anyNonInternalChildWidgets() const
{
    return false;
}

void RectWidget::setColor(const Color &color)
{
    m_color_topLeft = color;
    m_color_topRight = color;
    m_color_botRight = color;
    m_color_botLeft = color;
}

void RectWidget::setColors(const Color &color_top, const Color &color_bot)
{
    m_color_topLeft = color_top;
    m_color_topRight = color_top;
    m_color_botRight = color_bot;
    m_color_botLeft = color_bot;
}

void RectWidget::setColors(const Color &color_topLeft, const Color &color_topRight, const Color &color_botRight, const Color &color_botLeft)
{
    m_color_topLeft = color_topLeft;
    m_color_topRight = color_topRight;
    m_color_botRight = color_botRight;
    m_color_botLeft = color_botLeft;
}

} // namespace GUI
} // namespace engine
