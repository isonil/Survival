#ifndef ENGINE_GUI_EDIT_BOX_HPP
#define ENGINE_GUI_EDIT_BOX_HPP

#include "../../util/Range.hpp"
#include "Widget.hpp"

#include <memory>
#include <functional>

namespace engine
{
namespace GUI
{

class EditBox : public Widget
{
public:
    EditBox(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void onEvent(Event &event) override;
    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

private:
    bool m_enteringText;
    std::string m_text;
    bool m_selectingText;
    IntRange m_selectedText;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_EDIT_BOX_HPP
