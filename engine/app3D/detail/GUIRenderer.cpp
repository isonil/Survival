#include "GUIRenderer.hpp"

#include "../../GUI/IGUITexture.hpp"
#include "../../util/Color.hpp"
#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../../util/RichText.hpp"
#include "../ext/CGUITTFont.h"
#include "../managers/ResourcesManager.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"
#include "GUITexture.hpp"

namespace engine
{
namespace app3D
{
namespace detail
{

GUIRenderer::GUIRenderer(Device &device)
    : m_device{device},
      m_normalFont{},
      m_mediumFont{},
      m_bigFont{},
      m_shadow{},
      m_closeButton{}
{
    TRACK;

    reloadIrrObjects();
}

void GUIRenderer::dropIrrObjects()
{
    m_normalFont = nullptr;
    m_mediumFont = nullptr;
    m_bigFont = nullptr;
    m_shadow = nullptr;
    m_closeButton = nullptr;

    // TODO: drop irrlicht resources in all textures
}

void GUIRenderer::reloadIrrObjects()
{
    ResourcesManager &rmg = m_device.getResourcesManager();

    m_normalFont = &rmg.loadIrrFont("defaultFont.ttf", 12);
    m_mediumFont = &rmg.loadIrrFont("defaultFontBold.ttf", 15);
    m_bigFont = &rmg.loadIrrFont("defaultFontBold.ttf", 20);
    m_shadow = &rmg.loadIrrTexture(k_GUITexturesDirectory + "shadow.png", false);
    m_closeButton = &rmg.loadIrrTexture(k_GUITexturesDirectory + "closeButton.png", false);

    // TODO: reload irrlicht resources in all textures
}

void GUIRenderer::update()
{
    TRACK;

    for(size_t i = 0; i < m_textures.size();) {
        if(m_textures[i].expired()) {
            std::swap(m_textures[i], m_textures.back());
            m_textures.pop_back();
        }
        else
            ++i;
    }
}

std::shared_ptr <GUI::IGUITexture> GUIRenderer::getTexture(const std::string &path)
{
    const auto &texture = std::make_shared <GUITexture> (path, m_device.getPtr());
    m_textures.emplace_back(texture);
    return texture;
}

void GUIRenderer::drawLine(const IntVec2 &from, const IntVec2 &to, const Color &color) const
{
    const auto &irrFrom = IrrlichtConversions::toVector(from);
    const auto &irrTo = IrrlichtConversions::toVector(to);
    const auto &irrColor = IrrlichtConversions::toColor(color);

    m_device.getIrrDevice().getVideoDriver()->draw2DLine(irrFrom, irrTo, irrColor);
}

void GUIRenderer::drawFilledRect(const IntRect &rect, const Color &color) const
{
    const auto &irrRect = IrrlichtConversions::toRect(rect);
    const auto &irrColor = IrrlichtConversions::toColor(color);

    m_device.getIrrDevice().getVideoDriver()->draw2DRectangle(irrRect, irrColor, irrColor, irrColor, irrColor);
}

void GUIRenderer::drawRectOutline(const IntRect &rect, const Color &color) const
{
    const auto &irrRect = IrrlichtConversions::toRect(rect);
    const auto &irrColor = IrrlichtConversions::toColor(color);

    m_device.getIrrDevice().getVideoDriver()->draw2DRectangleOutline(irrRect, irrColor);
}

void GUIRenderer::drawGradientRect(const IntRect &rect, const Color &topLeft, const Color &topRight, const Color &botRight, const Color &botLeft) const
{
    const auto &irrRect = IrrlichtConversions::toRect(rect);
    const auto &irrColorTL = IrrlichtConversions::toColor(topLeft);
    const auto &irrColorTR = IrrlichtConversions::toColor(topRight);
    const auto &irrColorBR = IrrlichtConversions::toColor(botRight);
    const auto &irrColorBL = IrrlichtConversions::toColor(botLeft);

    m_device.getIrrDevice().getVideoDriver()->draw2DRectangle(irrRect, irrColorTL, irrColorTR, irrColorBL, irrColorBR);
}

void GUIRenderer::drawText(const std::string &text, const IntVec2 &pos, const Color &color, FontSize fontSize) const
{
    if(text.empty())
        return;

    const auto &size = getTextSize(text, fontSize);

    irr::core::recti irrRect{pos.x, pos.y, pos.x + size.x, pos.y + size.y};

    const auto &irrColor = IrrlichtConversions::toColor(color);

    getFont(fontSize).draw(text.c_str(), irrRect, irrColor);
}

void GUIRenderer::drawText(const std::string &text, const IntRect &rect, const Color &color, FontSize fontSize, HorizontalTextAlign horizontal, VerticalTextAlign vertical) const
{
    if(text.empty())
        return;

    int x{rect.pos.x};
    int y{rect.pos.y};
    int w{rect.size.x};
    int h{rect.size.y};

    const auto &size = getTextSize(text, fontSize);

    if(horizontal == HorizontalTextAlign::Right) {
        x = rect.getMaxX() - size.x;
        w -= rect.size.x - size.x;
    }

    if(vertical == VerticalTextAlign::Bottom) {
        y = rect.getMaxY() - size.y;
        h -= rect.size.y - size.y;
    }

    if(w <= 0.f || h <= 0.f)
        return;

    bool hCenter{horizontal == HorizontalTextAlign::Center};
    bool vCenter{vertical == VerticalTextAlign::Middle};

    const auto &irrColor = IrrlichtConversions::toColor(color);

    getFont(fontSize).draw(text.c_str(), {x, y, x + w, y + h}, irrColor, hCenter, vCenter);
}

void GUIRenderer::drawText(const RichText &richText, const IntVec2 &pos, FontSize fontSize) const
{
    if(richText.isEmpty())
        return;

    auto &font = getFont(fontSize);

    int currX{pos.x};
    int currY{pos.y};

    const auto &parts = richText.getParts();

    for(const auto &part : parts) {
        const std::string &text = part.getText();

        int curIndex{};

        while(true) {
            if(curIndex >= static_cast <int> (text.size()))
                break;

            auto newlinePos = text.find('\n', curIndex);

            if(newlinePos != std::string::npos) {
                const auto &textPart = text.substr(curIndex, newlinePos);

                drawText(textPart, {currX, currY}, part.getColor(), fontSize);
                currX = pos.x;
                currY += font.getDimension(textPart).Height;

                curIndex += textPart.size() + 1;
            }
            else {
                const auto &textPart = text.substr(curIndex);

                drawText(textPart, {currX, currY}, part.getColor(), fontSize);
                currX += font.getDimension(textPart).Width;

                break;
            }
        }
    }
}

int GUIRenderer::drawTextWordWrap(const std::string &text, const IntRect &rect, const Color &color, FontSize fontSize) const
{
    size_t curIndex{};
    int curY{rect.pos.y};

    while(curIndex != text.size()) {
        m_ossWorkingVar.str("");

        int printThisManyWords{};
        bool exceededWidth{};

        for(size_t i = curIndex; i < text.size(); ++i) {
            if(text[i] == ' ') {
                if(getTextSize(m_ossWorkingVar.str(), fontSize).x > rect.size.x) {
                    exceededWidth = true;
                    break;
                }
                else
                    ++printThisManyWords;
            }

            m_ossWorkingVar << text[i];
        }

        // check the last word
        if(!exceededWidth && getTextSize(m_ossWorkingVar.str(), fontSize).x <= rect.size.x)
            ++printThisManyWords;

        // if not even 1 word would fit, then just print the first word
        if(!printThisManyWords)
            printThisManyWords = 1;

        m_ossWorkingVar.str("");

        int gotThisManyWords{};

        for(size_t i = curIndex; i < text.size(); ++i) {
            if(text[i] == ' ') {
                ++gotThisManyWords;

                if(gotThisManyWords == printThisManyWords)
                    break;
            }

            m_ossWorkingVar << text[i];

            ++curIndex;
        }

        drawText(m_ossWorkingVar.str(), {rect.pos.x, curY}, color, fontSize);

        curY += getTextSize(m_ossWorkingVar.str(), fontSize).y;

        // skip all spaces at the end of the line
        while(curIndex < text.size() && text[curIndex] == ' ')
            ++curIndex;
    }

    return curY - rect.pos.y;
}

IntVec2 GUIRenderer::getTextSize(const std::string &text, FontSize fontSize) const
{
    const auto &size = getFont(fontSize).getDimension(text.c_str());

    return {static_cast <int> (size.Width),
            static_cast <int> (size.Height)};
}

void GUIRenderer::drawCloseButton(const IntRect &rect) const
{
    E_DASSERT(m_closeButton, "Close button texture is nullptr.");

    auto &drv = *m_device.getIrrDevice().getVideoDriver();

    const auto &irrRect = IrrlichtConversions::toRect(rect);

    irr::core::recti fullRect{0, 0,
                              static_cast <irr::s32> (m_closeButton->getSize().Width),
                              static_cast <irr::s32> (m_closeButton->getSize().Height)};

    drv.draw2DImage(m_closeButton, irrRect, fullRect, nullptr, nullptr, true);
}

void GUIRenderer::drawWarningIcon(const IntRect &) const
{
}

void GUIRenderer::drawErrorIcon(const IntRect &) const
{
}

void GUIRenderer::drawShadow(const IntRect &rect, const Color &color) const
{
    E_DASSERT(m_shadow, "Shadow texture is nullptr.");

    auto &drv = *m_device.getIrrDevice().getVideoDriver();

    const auto &irrRect = IrrlichtConversions::toRect(rect);
    const auto &irrColor = IrrlichtConversions::toColor(color);

    irr::core::recti fullRect{0, 0,
                              static_cast <irr::s32> (m_shadow->getSize().Width),
                              static_cast <irr::s32> (m_shadow->getSize().Height)};

    irr::video::SColor irrColors[4]{irrColor, irrColor, irrColor, irrColor};

    drv.enableMaterial2D();
    drv.draw2DImage(m_shadow, irrRect, fullRect, nullptr, irrColors, true);
    drv.enableMaterial2D(false);
}

DefDatabase &GUIRenderer::getDefDatabase() const
{
    return m_device.getDefDatabase();
}

irr::gui::CGUITTFont &GUIRenderer::getFont(FontSize fontSize) const
{
    switch(fontSize) {
    case FontSize::Normal:
        E_DASSERT(m_normalFont, "Normal font is nullptr.");
        return *m_normalFont;

    case FontSize::Medium:
        E_DASSERT(m_mediumFont, "Medium font is nullptr.");
        return *m_mediumFont;

    case FontSize::Big:
        E_DASSERT(m_bigFont, "Big font is nullptr.");
        return *m_bigFont;

    default:
        throw Exception{"Font size not handled."};
    }
}

const std::string GUIRenderer::k_GUITexturesDirectory = "GUI/";

} // namespace detail
} // namespace app3D
} // namespace engine
