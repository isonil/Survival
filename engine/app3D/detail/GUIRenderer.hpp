#ifndef ENGINE_APP_3D_DETAIL_GUI_RENDERER_HPP
#define ENGINE_APP_3D_DETAIL_GUI_RENDERER_HPP

#include "../../GUI/IGUIRenderer.hpp"
#include "../../util/Trace.hpp"
#include "../../util/Rect.hpp"
#include "../../util/Vec2.hpp"
#include "../IIrrlichtObjectsHolder.hpp"

#include <irrlicht/irrlicht.h>

#include <string>

namespace irr { namespace gui { class CGUITTFont; } }
namespace engine { namespace app3D { class Device; } }
namespace engine { namespace GUI { class IGUITexture; } }

namespace engine
{
namespace app3D
{
namespace detail
{

class GUITexture;

class GUIRenderer : public GUI::IGUIRenderer, public IIrrlichtObjectsHolder, public Tracked <GUIRenderer>
{
public:
    GUIRenderer(Device &device);
    GUIRenderer(const GUIRenderer &) = delete;

    GUIRenderer &operator = (const GUIRenderer &) = delete;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    void update();

    std::shared_ptr <GUI::IGUITexture> getTexture(const std::string &path) override;
    void drawLine(const IntVec2 &from, const IntVec2 &to, const Color &color) const override;
    void drawFilledRect(const IntRect &rect, const Color &color) const override;
    void drawRectOutline(const IntRect &rect, const Color &color) const override;
    void drawGradientRect(const IntRect &rect, const Color &topLeft, const Color &topRight, const Color &botRight, const Color &botLeft) const override;
    void drawText(const std::string &text, const IntVec2 &pos, const Color &color, FontSize fontSize = FontSize::Normal) const override;
    void drawText(const std::string &text, const IntRect &rect, const Color &color, FontSize fontSize = FontSize::Normal, HorizontalTextAlign horizontal = HorizontalTextAlign::Left, VerticalTextAlign vertical = VerticalTextAlign::Top) const override;
    void drawText(const RichText &richText, const IntVec2 &pos, FontSize fontSize = FontSize::Normal) const override;
    int drawTextWordWrap(const std::string &text, const IntRect &rect, const Color &color, FontSize fontSize = FontSize::Normal) const override;
    IntVec2 getTextSize(const std::string &text, FontSize fontSize = FontSize::Normal) const override;
    void drawCloseButton(const IntRect &rect) const override;
    void drawWarningIcon(const IntRect &rect) const override;
    void drawErrorIcon(const IntRect &rect) const override;
    void drawShadow(const IntRect &rect, const Color &color) const override;

    DefDatabase &getDefDatabase() const override;

private:
    irr::gui::CGUITTFont &getFont(FontSize fontSize) const;

    static const std::string k_GUITexturesDirectory;

    Device &m_device;

    std::vector <std::weak_ptr <GUITexture>> m_textures;
    irr::gui::CGUITTFont *m_normalFont;
    irr::gui::CGUITTFont *m_mediumFont;
    irr::gui::CGUITTFont *m_bigFont;
    irr::video::ITexture *m_shadow;
    irr::video::ITexture *m_closeButton;

    // working vars
    mutable std::ostringstream m_ossWorkingVar;
};

} // namespace detail
} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_DETAIL_GUI_RENDERER_HPP
