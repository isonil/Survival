#ifndef ENGINE_GUI_IMAGE_HPP
#define ENGINE_GUI_IMAGE_HPP

#include "../../util/Color.hpp"
#include "Widget.hpp"

#include <memory>
#include <functional>

namespace engine
{
namespace GUI
{

class IGUITexture;

class Image : public Widget
{
public:
    Image(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    void setOnMouseOver(std::function <void()> onMouseOver);

    void setTexture(const std::shared_ptr <IGUITexture> &texture);
    void setColor(const Color &color);

private:
    std::shared_ptr <IGUITexture> m_texture;
    Color m_color;

    std::function <void()> m_onMouseOver;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_IMAGE_HPP
