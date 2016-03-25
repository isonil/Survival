#ifndef ENGINE_GUI_PROGESS_BAR_HPP
#define ENGINE_GUI_PROGESS_BAR_HPP

#include "Widget.hpp"

namespace engine
{
namespace GUI
{

class ProgressBar : public Widget
{
public:
    ProgressBar(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    void setProgress(float progress);

private:
    float m_progress;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_PROGESS_BAR_HPP
