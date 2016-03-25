#ifndef ENGINE_GUI_GUI_MANAGER_HPP
#define ENGINE_GUI_GUI_MANAGER_HPP

#include "../util/Trace.hpp"
#include "../util/Rect.hpp"
#include "widgets/WidgetContainer.hpp"

#include <memory>

namespace engine { class AppTime; }

namespace engine
{
namespace GUI
{

class IGUIRenderer;
class IGUITexture;

class GUIManager : public WidgetContainer, public Tracked <GUIManager>
{
public:
    GUIManager(const std::shared_ptr <IGUIRenderer> &renderer);

    void onEvent(Event &event) override;
    void draw() const override;

    void update(const AppTime &time);

private:
    void update(const AppTime &time, bool nonOccludedMouseOver) override;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_GUI_MANAGER_HPP
