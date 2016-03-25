#include "InventoryWindow.hpp"

#include "../../entities/character/Inventory.hpp"
#include "engine/GUI/widgets/Window.hpp"
#include "engine/GUI/widgets/Image.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/app3D/Device.hpp"
#include "../../itemContainers/MultiSlotItemContainer.hpp"
#include "../../itemContainers/SingleSlotItemContainer.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../widgets/MultiSlotItemContainerWidget.hpp"
#include "../widgets/SingleSlotItemContainerWidget.hpp"
#include "../MainGUI.hpp"

namespace app
{

InventoryWindow::InventoryWindow(Inventory &inventory)
{
    TRACK;

    auto &device = Global::getCore().getDevice();
    auto &GUIManager = device.getGUIManager();
    auto &GUIRenderer = GUIManager.getRenderer();
    const auto &screenSize = device.getScreenSize();

    m_window = GUIManager.addWindow({(screenSize - k_size) / 2, k_size});
    m_window->setTitle(k_title);

    const auto &multiSlotItemContainerPtr = inventory.getMultiSlotItemContainerPtr();

    engine::IntRect rect{{330, 55}, multiSlotItemContainerPtr->getSize() * MultiSlotItemContainerWidget::k_slotSize};
    m_multiSlotItemContainerWidget = m_window->addCustomWidget <MultiSlotItemContainerWidget> (rect, multiSlotItemContainerPtr);

    const auto &silhouetteTexture = GUIRenderer.getTexture(k_silhouetteTexturePath);
    m_silhouette = m_window->addImage({{80, 75}, silhouetteTexture->getSize()});
    m_silhouette->setTexture(silhouetteTexture);

    int bigSlotSize{SingleSlotItemContainerWidget::k_slotSize};

    m_handsItemContainerWidget = m_window->addCustomWidget <SingleSlotItemContainerWidget> ({233, 241, bigSlotSize, bigSlotSize}, inventory.getHandsItemContainerPtr(), "hands");
    m_armorItemContainerWidget = m_window->addCustomWidget <SingleSlotItemContainerWidget> ({40, 155, bigSlotSize, bigSlotSize}, inventory.getArmorItemContainerPtr(), "armor");
    m_specialItemContainerWidget = m_window->addCustomWidget <SingleSlotItemContainerWidget> ({233, 107, bigSlotSize, bigSlotSize}, inventory.getSpecialItemContainerPtr(), "special");
}

void InventoryWindow::update()
{
}

const std::shared_ptr <engine::GUI::Window> &InventoryWindow::getWindow() const
{
    E_DASSERT(m_window, "Window is nullptr.");
    return m_window;
}

const engine::IntVec2 InventoryWindow::k_size{650, 500};
const std::string InventoryWindow::k_title = "Inventory";
const std::string InventoryWindow::k_silhouetteTexturePath = "GUI/silhouette.png";

} // namespace app
