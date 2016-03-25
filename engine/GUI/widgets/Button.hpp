#ifndef ENGINE_GUI_BUTTON_HPP
#define ENGINE_GUI_BUTTON_HPP

#include "Widget.hpp"

#include "../../app3D/Sound.hpp"

#include <memory>
#include <functional>

namespace engine
{
namespace GUI
{

class IGUITexture;

class Button : public Widget
{
public:
    Button(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void onEvent(Event &event) override;
    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    void setTexture(const std::shared_ptr <IGUITexture> &texture);
    void setText(const std::string &text);
    void setDrawBackground(bool val);

    void setOnPressed(std::function <void()> onPressed);

private:
    static const float k_mouseOverAccumulatorFactor;
    static const std::string k_clickSoundDefName;

    app3D::Sound m_clickSound;
    float m_mouseOverAccumulator;
    bool m_pressed;
    std::function <void()> m_onPressed;
    std::string m_text;
    std::shared_ptr <IGUITexture> m_texture;
    bool m_drawBackground;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_BUTTON_HPP
