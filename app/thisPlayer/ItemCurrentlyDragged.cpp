#include "ItemCurrentlyDragged.hpp"

#include "../GUI/widgets/MultiSlotItemContainerWidget.hpp"
#include "engine/app3D/managers/CursorManager.hpp"
#include "../itemContainers/MultiSlotItemContainer.hpp"
#include "../entities/Item.hpp"
#include "../defs/ItemDef.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

ItemCurrentlyDragged::ItemCurrentlyDragged()
    : m_draggedStack{}
{
}

void ItemCurrentlyDragged::setItem(const std::weak_ptr <Item> &item, const engine::IntVec2 &offset, const std::weak_ptr <IItemContainer> &itemContainer)
{
    TRACK;

    const auto &itemShared = item.lock();

    if(!itemShared)
        throw engine::Exception{"Tried to set nullptr item as item currently dragged."};

    setItem(item, offset, itemContainer, itemShared->getStack());
}

void ItemCurrentlyDragged::setItem(const std::weak_ptr <Item> &item, const engine::IntVec2 &offset, const std::weak_ptr <IItemContainer> &itemContainer, int draggedStack)
{
    TRACK;

    if(item.expired())
        throw engine::Exception{"Tried to set nullptr item as item currently dragged."};

    if(draggedStack <= 0)
        throw engine::Exception{"Tried to drag <= 0 stack."};

    if(draggedStack > item.lock()->getStack())
        throw engine::Exception{"Tried to drag bigger stack than available."};

    // itemContainer can be nullptr

    m_item = item;
    m_offset = offset;
    m_itemContainer = itemContainer;
    m_draggedStack = draggedStack;
}

void ItemCurrentlyDragged::removeItem()
{
    m_item.reset();
    m_draggedStack = 0;
}

void ItemCurrentlyDragged::itemHasBeenMoved(bool movedWholeStack)
{
    TRACK;

    if(!hasItem())
        throw engine::Exception{"itemHasBeenMoved called without any current item."};

    const auto &item = getItem();
    const auto &container = m_itemContainer.lock();

    // remove only if moved whole stack (otherwise it should have been split somewhere else)
    if(container && movedWholeStack)
        container->tryRemoveItem(item.getEntityID());

    removeItem();
}

bool ItemCurrentlyDragged::hasItem() const
{
    return !m_item.expired();
}

void ItemCurrentlyDragged::draw() const
{
    TRACK;

    if(!hasItem())
        return;

    auto &device = Global::getCore().getDevice();
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    const auto &cursorPos = device.getCursorManager().getCursorPosition();
    const auto &item = getItem();
    const auto &itemDef = item.getDef();
    const auto &texture = itemDef.getTextureInInventory();
    const auto &size = itemDef.getSizeInInventory() * MultiSlotItemContainerWidget::k_slotSize;

    engine::IntRect rect{cursorPos - getOffset(), size};

    texture.draw(rect);

    int stack{getDraggedStack()};

    if(stack > 1)
        GUIRenderer.drawText(std::to_string(stack), rect, engine::Color::k_white, engine::GUI::IGUIRenderer::FontSize::Normal, engine::GUI::IGUIRenderer::HorizontalTextAlign::Right, engine::GUI::IGUIRenderer::VerticalTextAlign::Bottom);
}

Item &ItemCurrentlyDragged::getItem() const
{
    const auto &shared = m_item.lock();

    if(!shared)
        throw engine::Exception{"Tried to get nullptr item from item currently dragged. This should have been checked before."};

    return *shared;
}

std::shared_ptr <Item> ItemCurrentlyDragged::getItemPtr() const
{
    const auto &shared = m_item.lock();

    if(!shared)
        throw engine::Exception{"Tried to get nullptr item from item currently dragged. This should have been checked before."};

    return shared;
}

const engine::IntVec2 &ItemCurrentlyDragged::getOffset() const
{
    return m_offset;
}

int ItemCurrentlyDragged::getDraggedStack() const
{
    if(!hasItem())
        return 0;

    return m_draggedStack;
}

} // namespace app
