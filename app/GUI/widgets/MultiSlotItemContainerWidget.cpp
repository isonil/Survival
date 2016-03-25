#include "MultiSlotItemContainerWidget.hpp"

#include "engine/GUI/widgets/WidgetContainer.hpp"
#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../defs/DefsCache.hpp"
#include "engine/app3D/managers/CursorManager.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/Event.hpp"
#include "engine/app3D/Device.hpp"
#include "../../entities/Item.hpp"
#include "../../defs/ItemDef.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../../SoundPool.hpp"
#include "../MainGUI.hpp"

namespace app
{

MultiSlotItemContainerWidget::MultiSlotItemContainerWidget(engine::GUI::WidgetContainer *parent, const std::shared_ptr <engine::GUI::IGUIRenderer> &renderer, const engine::IntRect &rect, const std::shared_ptr <MultiSlotItemContainer> &itemContainer)
    : Widget{parent, renderer, rect},
      m_itemContainer{itemContainer}
{
    if(!m_itemContainer)
        throw engine::Exception{"Item container is nullptr."};

    m_slotTexture = getRenderer().getTexture(k_slotTexturePath);
}

void MultiSlotItemContainerWidget::onEvent(engine::GUI::Event &event)
{
    TRACK;

    base::onEvent(event);

    if(event.getType() == engine::GUI::Event::Type::MouseEvent) {
        if(event.getMouseButton() == engine::GUI::Event::MouseButton::Left) {
            auto action = event.getMouseAction();
            auto &core = Global::getCore();
            auto &defsCache = core.getDefsCache();
            auto &soundPool = core.getSoundPool();
            auto &itemCurrentlyDragged = core.getThisPlayer().getItemCurrentlyDragged();
            const auto &pos = event.getPosition();

            if(action == engine::GUI::Event::MouseAction::Down) {
                const auto &item = tryGetItemAtMousePos(pos);
                const auto &screenRect = getScreenRect();

                if(item && !itemCurrentlyDragged.hasItem()) {
                    E_DASSERT(item->item, "Item is nullptr.");

                    int countToTake{item->item->getStack()};

                    if(event.isShift() && countToTake >= 2)
                        countToTake = countToTake / 2;

                    if(event.isCtrl())
                        countToTake = 1;

                    const auto &offset = pos - (item->position * k_slotSize + screenRect.pos);
                    itemCurrentlyDragged.setItem(item->item, offset, m_itemContainer, countToTake);

                    soundPool.play(defsCache.Sound_Inventory1);
                }
            }
            else if(action == engine::GUI::Event::MouseAction::Up) {
                if(itemCurrentlyDragged.hasItem() && isNonOccludedAt(pos)) {
                    E_DASSERT(m_itemContainer, "Item container is nullptr.");

                    const auto &slotTilePos = getFreeSlotTileForItemCurrentlyDraggedAssumingNonOccluded(pos);

                    if(slotTilePos) {
                        int stackBefore{itemCurrentlyDragged.getItem().getStack()};
                        int moved{m_itemContainer->tryAddItem(itemCurrentlyDragged.getItemPtr(), *slotTilePos, itemCurrentlyDragged.getDraggedStack())};

                        if(moved) {
                            bool movedWholeStack{stackBefore == moved};
                            itemCurrentlyDragged.itemHasBeenMoved(movedWholeStack);
                        }

                        soundPool.play(defsCache.Sound_Inventory2);
                    }
                }
            }
        }
    }
}

void MultiSlotItemContainerWidget::update(const engine::AppTime &time, bool nonOccludedMouseOver)
{
    TRACK;

    auto &core = Global::getCore();
    const auto &itemCurrentlyDragged = core.getThisPlayer().getItemCurrentlyDragged();

    m_highlightRect = {};

    if(nonOccludedMouseOver) {
        const auto &cursorPos = core.getDevice().getCursorManager().getCursorPosition();

        if(itemCurrentlyDragged.hasItem()) {
            const auto &tile = getFreeSlotTileForItemCurrentlyDraggedAssumingNonOccluded(cursorPos);

            if(tile) {
                const auto &size = itemCurrentlyDragged.getItem().getDef().getSizeInInventory();
                m_highlightRect = {*tile, size};
            }
        }
        else {
            const auto &item = tryGetItemAtMousePos(cursorPos);

            if(item) {
                m_highlightRect = item->getRect();

                E_DASSERT(item->item, "Item is nullptr.");

                core.getMainGUI().getTooltip().setItemDef(item->item->getDefPtr());
            }
        }
    }
}

void MultiSlotItemContainerWidget::draw() const
{
    TRACK;

    E_DASSERT(m_itemContainer, "Item container is nullptr.");
    E_DASSERT(m_slotTexture, "Slot texture is nullptr.");

    const auto &GUIRenderer = getRenderer();
    const auto &screenRect = getScreenRect();
    const auto &size = m_itemContainer->getSize();
    const auto &itemCurrentlyDragged = Global::getCore().getThisPlayer().getItemCurrentlyDragged();

    Item *draggedItem{};

    if(itemCurrentlyDragged.hasItem())
        draggedItem = &itemCurrentlyDragged.getItem();

    engine::IntVec2 slotSize{k_slotSize, k_slotSize};

    for(int y = 0; y < size.y; ++y) {
        for(int x = 0; x < size.x; ++x) {
            const auto &pos = engine::IntVec2{x, y} * k_slotSize + screenRect.pos;
            m_slotTexture->draw({pos, slotSize});
        }
    }

    for(const auto &elem : m_itemContainer->getItems()) {
        E_DASSERT(elem.item, "Item is nullptr.");

        const auto &itemDef = elem.item->getDef();
        const auto &elemSizeInInventory = itemDef.getSizeInInventory();
        const auto &realPos = screenRect.pos + elem.position * slotSize;
        const auto &realSize = elemSizeInInventory * slotSize;

        auto color = engine::Color::k_white;
        int stack{elem.item->getStack()};
        bool draggingThisItem{};

        if(elem.item.get() == draggedItem) {
            color = {1.f, 1.f, 1.f, 0.5f};
            stack -= itemCurrentlyDragged.getDraggedStack();
            draggingThisItem = true;
        }

        engine::IntRect rect{realPos, realSize};

        itemDef.getTextureInInventory().draw(rect, color);

        if(stack > 1 || (draggingThisItem && stack == 1))
            GUIRenderer.drawText(std::to_string(stack), rect, color, engine::GUI::IGUIRenderer::FontSize::Normal, engine::GUI::IGUIRenderer::HorizontalTextAlign::Right, engine::GUI::IGUIRenderer::VerticalTextAlign::Bottom);
    }

    if(!m_highlightRect.isEmpty()) {
        const auto &realPos = screenRect.pos + m_highlightRect.pos * slotSize;
        const auto &realSize = m_highlightRect.size * slotSize;

        GUIRenderer.drawFilledRect({realPos, realSize}, engine::Color{1.f, 1.f, 1.f, 0.05f});
    }
}

bool MultiSlotItemContainerWidget::anyNonInternalChildWidgets() const
{
    return false;
}

const std::shared_ptr <MultiSlotItemContainer> &MultiSlotItemContainerWidget::getMultiSlotItemContainerPtr() const
{
    E_DASSERT(m_itemContainer, "Item container is nullptr.");
    return m_itemContainer;
}

const int MultiSlotItemContainerWidget::k_slotSize{45};

std::experimental::optional <MultiSlotItemContainer::ItemInContainer> MultiSlotItemContainerWidget::tryGetItemAtMousePos(const engine::IntVec2 &pos) const
{
    TRACK;

    if(!isNonOccludedAt(pos))
        return std::experimental::optional <MultiSlotItemContainer::ItemInContainer> {};

    E_DASSERT(m_itemContainer, "Item container is nullptr.");

    const auto &relPos = pos - getScreenRect().pos;

    const auto &items = m_itemContainer->getItems();
    for(const auto &elem : items) {
        const auto &rect = elem.getRect();
        if(engine::IntRect{rect.pos * k_slotSize, rect.size * k_slotSize}.contains(relPos))
            return elem;
    }

    return std::experimental::optional <MultiSlotItemContainer::ItemInContainer> {};
}

std::experimental::optional <engine::IntVec2> MultiSlotItemContainerWidget::getExactSlotTilePosAssumingNonOccluded(const engine::IntVec2 &pos) const
{
    TRACK;

    const auto &screenRect = getScreenRect();

    engine::IntVec2 ret{(pos.x - screenRect.pos.x) / k_slotSize,
                              (pos.y - screenRect.pos.y) / k_slotSize};

    E_DASSERT(m_itemContainer, "Item container is nullptr.");

    const auto &itemContainerSize = m_itemContainer->getSize();

    if(ret.x >= 0 && ret.y >= 0 && ret.x < itemContainerSize.x && ret.y < itemContainerSize.y)
        return ret;

    return std::experimental::optional <engine::IntVec2> {};
}

std::experimental::optional <engine::IntVec2> MultiSlotItemContainerWidget::getFreeSlotTile(const Item &item, const engine::IntVec2 &pos)
{
    TRACK;

    E_DASSERT(m_itemContainer, "Item container is nullptr.");

    if(m_itemContainer->wouldFit(item, pos))
        return pos;
    else if(m_itemContainer->wouldFit(item, pos.movedX(1)))
        return pos.movedX(1);
    else if(m_itemContainer->wouldFit(item, pos.movedX(-1)))
        return pos.movedX(-1);
    else if(m_itemContainer->wouldFit(item, pos.movedY(1)))
        return pos.movedY(1);
    else if(m_itemContainer->wouldFit(item, pos.movedY(-1)))
        return pos.movedY(-1);
    else
        return std::experimental::optional <engine::IntVec2> {};
}

std::experimental::optional <engine::IntVec2> MultiSlotItemContainerWidget::getFreeSlotTileForItemCurrentlyDraggedAssumingNonOccluded(const engine::IntVec2 &cursorPos)
{
    TRACK;

    const auto &itemCurrentlyDragged = Global::getCore().getThisPlayer().getItemCurrentlyDragged();

    if(!itemCurrentlyDragged.hasItem())
        return std::experimental::optional <engine::IntVec2> {};

    const auto &screenRect = getScreenRect();
    const auto &item = itemCurrentlyDragged.getItem();
    const auto &dragOffset = itemCurrentlyDragged.getOffset();
    const auto &tileUnderCursor = (cursorPos - screenRect.pos) / k_slotSize;

    const auto &pos = tileUnderCursor - dragOffset / k_slotSize;

    E_DASSERT(m_itemContainer, "Item container is nullptr.");

    const auto &currItem = m_itemContainer->tryGetItemAt(tileUnderCursor);

    // if there is the same object under cursor, then don't look for near free space

    if(currItem && currItem.get() == &item)
        return std::experimental::optional <engine::IntVec2> {};

    return getFreeSlotTile(item, pos);
}

const std::string MultiSlotItemContainerWidget::k_slotTexturePath = "GUI/slot.png";

} // namespace app
