#ifndef ENGINE_GUI_I_GUI_RENDERER_HPP
#define ENGINE_GUI_I_GUI_RENDERER_HPP

#include "../util/Rect.hpp"
#include "../util/Vec2.hpp"

namespace engine { class Color; class RichText; class DefDatabase; }

namespace engine
{
namespace GUI
{

class IGUITexture;

class IGUIRenderer
{
public:
    enum class FontSize
    {
        Normal,
        Medium,
        Big
    };

    enum class HorizontalTextAlign
    {
        Left,
        Center,
        Right
    };

    enum class VerticalTextAlign
    {
        Top,
        Middle,
        Bottom
    };

    virtual std::shared_ptr <IGUITexture> getTexture(const std::string &path) = 0;
    virtual void drawLine(const IntVec2 &from, const IntVec2 &to, const Color &color) const = 0;
    virtual void drawFilledRect(const IntRect &rect, const Color &color) const = 0;
    virtual void drawRectOutline(const IntRect &rect, const Color &color) const = 0;
    virtual void drawGradientRect(const IntRect &rect, const Color &topLeft, const Color &topRight, const Color &botRight, const Color &botLeft) const = 0;
    virtual void drawText(const std::string &text, const IntVec2 &pos, const Color &color, FontSize fontSize = FontSize::Normal) const = 0;
    virtual void drawText(const std::string &text, const IntRect &rect, const Color &color, FontSize fontSize = FontSize::Normal, HorizontalTextAlign horizontal = HorizontalTextAlign::Left, VerticalTextAlign vertical = VerticalTextAlign::Top) const = 0;
    virtual void drawText(const RichText &richText, const IntVec2 &pos, FontSize fontSize = FontSize::Normal) const = 0;
    virtual int drawTextWordWrap(const std::string &text, const IntRect &rect, const Color &color, FontSize fontSize = FontSize::Normal) const = 0;
    virtual IntVec2 getTextSize(const std::string &text, FontSize fontSize = FontSize::Normal) const = 0;
    virtual void drawCloseButton(const IntRect &rect) const = 0;
    virtual void drawWarningIcon(const IntRect &rect) const = 0;
    virtual void drawErrorIcon(const IntRect &rect) const = 0;
    virtual void drawShadow(const IntRect &rect, const Color &color) const = 0;

    virtual DefDatabase &getDefDatabase() const = 0;

    virtual ~IGUIRenderer() = default;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_I_GUI_RENDERER_HPP
