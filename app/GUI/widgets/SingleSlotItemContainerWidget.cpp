#include "SingleSlotItemContainerWidget.hpp"

#include "../../itemContainers/SingleSlotItemContainer.hpp"
#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../thisPlayer/ItemCurrentlyDragged.hpp"
#include "../../entities/Item.hpp"
#include "../../defs/ItemDef.hpp"
#include "../../defs/DefsCache.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../../SoundPool.hpp"
#include "engine/GUI/Event.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../MainGUI.hpp"
#include "MultiSlotItemContainerWidget.hpp"

namespace app
{

SingleSlotItemContainerWidget::SingleSlotItemContainerWidget(engine::GUI::WidgetContainer *parent, const std::shared_ptr <engine::GUI::IGUIRenderer> &renderer, const engine::IntRect &rect, const std::shared_ptr <SingleSlotItemContainer> &itemContainer, const std::string &text)
    : Widget{parent, renderer, rect},
      m_itemContainer{itemContainer},
      m_text{text},
      m_shouldHighlight{}
{
    if(!m_itemContainer)
        throw engine::Exception{"Item container is nullptr."};

    m_slotTexture = getRenderer().getTexture(k_slotTexturePath);
}

void SingleSlotItemContainerWidget::onEvent(engine::GUI::Event &event)
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
                E_DASSERT(m_itemContainer, "Item container is nullptr.");

                if(m_itemContainer->hasItem() && !itemCurrentlyDragged.hasItem()) {
                    const auto &itemContainerItemPtr = m_itemContainer->getItemPtr();

                    E_DASSERT(itemContainerItemPtr, "Item is nullptr.");

                    int countToTake{itemContainerItemPtr->getStack()};

                    if(event.isShift() && countToTake >= 2)
                        countToTake = countToTake / 2;

                    if(event.isCtrl())
                        countToTake = 1;

                    const auto &offset = (itemContainerItemPtr->getDef().getSizeInInventory() * MultiSlotItemContainerWidget::k_slotSize) / 2;
                    itemCurrentlyDragged.setItem(itemContainerItemPtr, offset, m_itemContainer, countToTake);

                    soundPool.play(defsCache.Sound_Inventory1);
                }
            }
            else if(action == engine::GUI::Event::MouseAction::Up) {
                if(itemCurrentlyDragged.hasItem() && isNonOccludedAt(pos)) {
                    E_DASSERT(m_itemContainer, "Item container is nullptr.");

                    const auto &draggedItem = itemCurrentlyDragged.getItem();

                    if(m_itemContainer->wouldFit(draggedItem)) {
                        int stackBefore{draggedItem.getStack()};
                        int moved{m_itemContainer->tryAddItem(itemCurrentlyDragged.getItemPtr(), itemCurrentlyDragged.getDraggedStack())};

                        if(moved) {
                            bool movedWholeStack{stackBefore == moved};
                            itemCurrentlyDragged.itemHasBeenMoved(movedWholeStack);

                            if(m_itemContainer->getSlotType() == SingleSlotItemContainer::SlotType::Hands &&
                               m_itemContainer->getItem().getDef().getOnUsed().isWeapon())
                                soundPool.play(defsCache.Sound_EquipWeapon);
                            else
                                soundPool.play(defsCache.Sound_Inventory2);
                        }
                    }
                }
            }
        }
    }
}

void SingleSlotItemContainerWidget::update(const engine::AppTime &time, bool nonOccludedMouseOver)
{
    TRACK;

    auto &core = Global::getCore();
    const auto &itemCurrentlyDragged = core.getThisPlayer().getItemCurrentlyDragged();

    m_shouldHighlight = false;

    if(nonOccludedMouseOver) {
        E_DASSERT(m_itemContainer, "Item container is nullptr.");

        if(itemCurrentlyDragged.hasItem()) {
            const auto &draggedItem = itemCurrentlyDragged.getItem();

            if(m_itemContainer->wouldFit(draggedItem))
                m_shouldHighlight = true;
        }
        else if(m_itemContainer->hasItem()) {
            m_shouldHighlight = true;
            core.getMainGUI().getTooltip().setItemDef(m_itemContainer->getItem().getDefPtr());
        }
    }
}

void SingleSlotItemContainerWidget::draw() const
{
    TRACK;

    const auto &GUIRenderer = getRenderer();
    const auto &screenRect = getScreenRect();
    engine::IntVec2 slotSize{k_slotSize, k_slotSize};

    E_DASSERT(m_slotTexture, "Slot texture is nullptr.");

    m_slotTexture->draw({screenRect.pos, slotSize});

    if(m_itemContainer->hasItem()) {
        const auto &itemCurrentlyDragged = Global::getCore().getThisPlayer().getItemCurrentlyDragged();
        auto &item = m_itemContainer->getItem();
        const auto &tex = item.getDef().getTextureInInventory();

        auto color = engine::Color::k_white;
        int stack{item.getStack()};
        bool draggingThisItem{};

        if(itemCurrentlyDragged.hasItem() && &itemCurrentlyDragged.getItem() == &item) {
            color = {1.f, 1.f, 1.f, 0.5f};
            stack -= itemCurrentlyDragged.getDraggedStack();
            draggingThisItem = true;
        }

        const auto &rect = engine::IntRect{screenRect.pos, tex.getSize()}.fittedToSquareWithAspect(screenRect.pos, k_slotSize);

        tex.draw(rect, color);

        if(stack > 1 || (draggingThisItem && stack == 1)) {
            engine::IntRect textRect{screenRect.pos, {k_slotSize, k_slotSize}};
            GUIRenderer.drawText(std::to_string(stack), textRect, color, engine::GUI::IGUIRenderer::FontSize::Normal, engine::GUI::IGUIRenderer::HorizontalTextAlign::Right, engine::GUI::IGUIRenderer::VerticalTextAlign::Bottom);
        }
    }
    else
        GUIRenderer.drawText(m_text, {screenRect.pos, slotSize}, {25, 25, 25}, engine::GUI::IGUIRenderer::FontSize::Normal, engine::GUI::IGUIRenderer::HorizontalTextAlign::Center, engine::GUI::IGUIRenderer::VerticalTextAlign::Middle);

    if(m_shouldHighlight)
        GUIRenderer.drawFilledRect({screenRect.pos, slotSize}, {1.f, 1.f, 1.f, 0.05f});
}

bool SingleSlotItemContainerWidget::anyNonInternalChildWidgets() const
{
    return false;
}

const int SingleSlotItemContainerWidget::k_slotSize{60};
const std::string SingleSlotItemContainerWidget::k_slotTexturePath = "GUI/bigSlot.png";

} // namespace app
