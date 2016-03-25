#ifndef ENGINE_GUI_RECT_WIDGET_HPP
#define ENGINE_GUI_RECT_WIDGET_HPP

#include "Widget.hpp"

#include "../../util/Color.hpp"

#include <memory>
#include <functional>

namespace engine
{
namespace GUI
{

class IGUITexture;

class RectWidget : public Widget
{
public:
    RectWidget(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    void setColor(const Color &color);
    void setColors(const Color &color_top, const Color &color_bot);
    void setColors(const Color &color_topLeft, const Color &color_topRight, const Color &color_botRight, const Color &color_botLeft);

private:
    Color m_color_topLeft, m_color_topRight, m_color_botRight, m_color_botLeft;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_RECT_WIDGET_HPP
