#include "Tooltip.hpp"

#include "../../defs/UpgradeDef.hpp"
#include "../../defs/ItemDef.hpp"
#include "engine/app3D/managers/CursorManager.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"

namespace app
{

Tooltip::Tooltip()
    : m_alreadyDrawn{},
      m_infoType{InfoType::None}
{
}

void Tooltip::update()
{
    if(m_alreadyDrawn)
        m_infoType = InfoType::None;
}

void Tooltip::draw()
{
    if(m_infoType == InfoType::None)
        return;

    auto &device = Global::getCore().getDevice();
    auto &renderer = device.getGUIManager().getRenderer();
    const auto &cursorPos = device.getCursorManager().getCursorPosition();
    const auto &screenSize = device.getScreenSize();

    engine::IntVec2 size;

    if(m_infoType == InfoType::Text)
        size = renderer.getTextSize(m_text) + engine::IntVec2{20, 20};
    else if(m_infoType == InfoType::UpgradeDef)
        size = {300, 200};
    else if(m_infoType == InfoType::ItemDef)
        size = {300, 200};

    engine::IntRect rect{cursorPos, size};

    if(rect.getMaxX() > screenSize.x)
        rect.pos.x -= rect.size.x;

    if(rect.getMaxY() > screenSize.y)
        rect.pos.y -= rect.size.y;

    renderer.drawFilledRect(rect, engine::Color::k_black.changedAlpha(0.3f));

    engine::IntVec2 curPos{rect.pos.moved(10, 10)};

    if(m_infoType == InfoType::Text)
        renderer.drawText(m_text, curPos, engine::Color::k_white);
    else if(m_infoType == InfoType::UpgradeDef) {
        const auto &upgradeDef = m_upgradeDef.lock();

        if(upgradeDef) {
            const auto &capitalizedLabel = upgradeDef->getCapitalizedLabel();

            renderer.drawText(capitalizedLabel, curPos, engine::Color::k_white);

            if(upgradeDef->getRequiredUpgradePoints()) {
                int width{renderer.getTextSize(capitalizedLabel).x};
                renderer.drawText('(' + std::to_string(upgradeDef->getRequiredUpgradePoints()) + " pts)", curPos.movedX(width + 7), {0.5f, 0.5f, 1.f});
            }

            curPos.y += 18;

            curPos.y += renderer.drawTextWordWrap(upgradeDef->getDescription(), {curPos, rect.size.moved(-20, -20)}, {0.8f, 0.8f, 0.8f});
            curPos.y += 5;

            upgradeDef->drawEffects(curPos);
        }
    }
    else if(m_infoType == InfoType::ItemDef) {
        const auto &itemDef = m_itemDef.lock();

        if(itemDef) {
            const auto &capitalizedLabel = itemDef->getCapitalizedLabel();

            renderer.drawText(capitalizedLabel, curPos, engine::Color::k_white);

            curPos.y += 18;

            itemDef->getTextureInInventory().draw(curPos);
        }
    }

    m_alreadyDrawn = true;
}

void Tooltip::setText(const std::string &str)
{
    m_infoType = InfoType::Text;
    m_text = str;
    m_alreadyDrawn = false;
}

void Tooltip::setUpgradeDef(const std::weak_ptr <UpgradeDef> &upgradeDef)
{
    if(upgradeDef.expired()) {
        m_infoType = InfoType::None;
        return;
    }

    m_infoType = InfoType::UpgradeDef;
    m_upgradeDef = upgradeDef;
    m_alreadyDrawn = false;
}

void Tooltip::setItemDef(const std::weak_ptr <ItemDef> &itemDef)
{
    if(itemDef.expired()) {
        m_infoType = InfoType::None;
        return;
    }

    m_infoType = InfoType::ItemDef;
    m_itemDef = itemDef;
    m_alreadyDrawn = false;
}

} // namespace app
