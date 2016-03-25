#include "ItemContainerWindow.hpp"

#include "../../itemContainers/MultiSlotItemContainer.hpp"
#include "engine/GUI/widgets/Window.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../widgets/MultiSlotItemContainerWidget.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"

namespace app
{

ItemContainerWindow::ItemContainerWindow(const std::shared_ptr <MultiSlotItemContainer> &itemContainer, const std::shared_ptr <engine::GUI::Window> &optionalInventoryWindowToSnapTo)
{
    if(!itemContainer)
        throw engine::Exception{"Item container is nullptr."};

    auto &device = Global::getCore().getDevice();
    const auto &screenSize = device.getScreenSize();

    engine::IntVec2 desiredWidgetSize{itemContainer->getSize().x * MultiSlotItemContainerWidget::k_slotSize,
                                      itemContainer->getSize().y * MultiSlotItemContainerWidget::k_slotSize};

    engine::IntVec2 desiredWindowSize{desiredWidgetSize.x + 50, desiredWidgetSize.y + 75};

    engine::IntVec2 desiredWindowPos;

    if(optionalInventoryWindowToSnapTo) {
        const int distanceBetweenWindows = 10;

        const auto &inventoryWindowScreenRect = optionalInventoryWindowToSnapTo->getScreenRect();

        engine::IntRect fullInclusiveRect{{}, {desiredWindowSize.x + inventoryWindowScreenRect.size.x + distanceBetweenWindows,
                                               std::max(desiredWindowSize.y, inventoryWindowScreenRect.size.y)}};

        fullInclusiveRect.pos = (screenSize - fullInclusiveRect.size) / 2;

        desiredWindowPos = fullInclusiveRect.pos;

        optionalInventoryWindowToSnapTo->setPosition({fullInclusiveRect.getMaxX() - inventoryWindowScreenRect.size.x,
                                                      fullInclusiveRect.pos.y});
    }
    else
        desiredWindowPos = (screenSize - desiredWindowSize) / 2;

    m_window = device.getGUIManager().addWindow({desiredWindowPos, desiredWindowSize});
    m_multiSlotItemContainerWidget = m_window->addCustomWidget <MultiSlotItemContainerWidget> (engine::IntRect{{25, 50}, desiredWidgetSize}, itemContainer);
}

void ItemContainerWindow::update(const std::string &title, const std::shared_ptr <MultiSlotItemContainer> &itemContainer)
{
    if(!itemContainer)
        throw engine::Exception{"Tried to update item container window with nullptr item container."};

    E_DASSERT(m_window, "Window is nullptr.");

    m_window->setTitle(title);

    if(itemContainer != m_multiSlotItemContainerWidget->getMultiSlotItemContainerPtr()) {
        engine::IntVec2 desiredWidgetSize{itemContainer->getSize().x * MultiSlotItemContainerWidget::k_slotSize,
                                          itemContainer->getSize().y * MultiSlotItemContainerWidget::k_slotSize};

        m_multiSlotItemContainerWidget = m_window->addCustomWidget <MultiSlotItemContainerWidget> (engine::IntRect{{25, 50}, desiredWidgetSize}, itemContainer);
    }
}

bool ItemContainerWindow::isClosed() const
{
    E_DASSERT(m_window, "Window is nullptr.");
    return m_window->isDead();
}

} // namespace app
