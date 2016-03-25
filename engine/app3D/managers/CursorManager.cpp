#include "CursorManager.hpp"

#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"
#include "ResourcesManager.hpp"

namespace engine
{
namespace app3D
{

CursorManager::CursorManager(Device &device)
    : m_device{device},
      m_cursorSpriteBank{}
{
    const auto &screenSize = m_device.getScreenSize();

    m_cursorPosBeforeLocking.set(screenSize.x / 2, screenSize.y / 2);

    reloadIrrObjects();

    showCursor(false);
}

void CursorManager::showCursor(bool show)
{
    auto &cursorControl = *m_device.getIrrDevice().getCursorControl();

    if(show) {
        cursorControl.setPosition(m_cursorPosBeforeLocking.x, m_cursorPosBeforeLocking.y);
        cursorControl.setVisible(true);
    }
    else {
        const auto &screenSize = m_device.getScreenSize();
        const auto &center = IrrlichtConversions::toVector(screenSize / 2);
        const auto &cursorPos = cursorControl.getPosition();

        m_cursorPosBeforeLocking.set(cursorPos.X, cursorPos.Y);

        cursorControl.setVisible(false);
        cursorControl.setPosition(center);
    }
}

IntVec2 CursorManager::getCursorPosition() const
{
    const auto &cursorPos = m_device.getIrrDevice().getCursorControl()->getPosition();
    return {cursorPos.X, cursorPos.Y};
}

void CursorManager::dropIrrObjects()
{
    m_cursorSpriteBank = nullptr;
}

void CursorManager::reloadIrrObjects()
{
    m_cursorSpriteBank = m_device.getIrrDevice().getGUIEnvironment()->addEmptySpriteBank("cursors");

    if(!m_cursorSpriteBank)
        throw Exception{"Could not create cursor sprite bank."};

    auto &normalCursor = m_device.getResourcesManager().loadIrrTexture(k_normalCursorTexturePath, false);
    m_cursorSpriteBank->addTextureAsSprite(&normalCursor);

    irr::gui::SCursorSprite cursorSprite{m_cursorSpriteBank, 0};

    m_device.getIrrDevice().getCursorControl()->changeIcon(irr::gui::ECI_NORMAL, cursorSprite);
}

const std::string CursorManager::k_normalCursorTexturePath = "GUI/cursor.png";

} // namespace app3D
} // namespace engine
