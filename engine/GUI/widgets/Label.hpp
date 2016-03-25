#ifndef ENGINE_GUI_LABEL_HPP
#define ENGINE_GUI_LABEL_HPP

#include "Widget.hpp"

#include "../../util/Color.hpp"
#include "../../util/RichText.hpp"

#include <memory>
#include <functional>

namespace engine
{
namespace GUI
{

class IGUITexture;

class Label : public Widget
{
public:
    Label(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect);

    void update(const AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    void setCentered(bool hCentered, bool vCentered);
    void setText(const std::string &text);
    void setText(const RichText &text);
    void setColor(const Color &color);

private:
    std::string m_text;
    RichText m_richText;
    Color m_color;
    bool m_hCentered;
    bool m_vCentered;
    bool m_useRichText;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_LABEL_HPP
