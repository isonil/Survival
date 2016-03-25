#ifndef ENGINE_GUI_WINDOW_HPP
#define ENGINE_GUI_WINDOW_HPP

#include "../../util/Vec2.hpp"
#include "WidgetContainer.hpp"

#include <functional>
#include <memory>

namespace engine
{
namespace GUI
{

class Window : public WidgetContainer
{
public:
    Window(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void onEvent(Event &event) override;
    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool moveToFrontOnFocus() const override;
    bool anyNonInternalChildWidgets() const override;

    void setTitle(const std::string &title);

private:
    IntRect getTitleBarScreenRect() const;

    static const std::string k_closeButtonTexturePath;
    static const float k_mouseOverAccumulatorFactor;

    std::string m_title;
    float m_mouseOverAccumulator;
    bool m_dragging;
    IntVec2 m_draggingOffset;
    std::shared_ptr <Button> m_closeButton;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_WINDOW_HPP
