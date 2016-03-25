#include "HandyWindow_Inventory.hpp"

#include "../../itemContainers/SingleSlotItemContainer.hpp"
#include "../../itemContainers/MultiSlotItemContainer.hpp"
#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../entities/Item.hpp"
#include "../../entities/Character.hpp"
#include "../../defs/DefsCache.hpp"
#include "../../defs/ItemDef.hpp"
#include "../../Core.hpp"
#include "../../Global.hpp"
#include "../../SoundPool.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/Event.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/util/Color.hpp"

namespace app
{

HandyWindow_Inventory::HandyWindow_Inventory()
    : HandyWindow{k_size},
      m_equippableItemsOffset{}
{
    TRACK;

    auto &GUIRenderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    m_upArrowTexture = GUIRenderer.getTexture(k_upArrowTexturePath);
    m_downArrowTexture = GUIRenderer.getTexture(k_downArrowTexturePath);
}

void HandyWindow_Inventory::update()
{
    TRACK;

    updateCurrentEquippableItems();
    updateEquippableItemsOffset();
}

void HandyWindow_Inventory::onEvent(engine::GUI::Event &event)
{
    base::onEvent(event);

    switch(event.getType()) {
    case engine::GUI::Event::Type::KeyboardEvent:
        switch(event.getKeyCode()) {
        case irr::KEY_DOWN:
            tryChangeSelection(false);
            break;

        case irr::KEY_UP:
            tryChangeSelection(true);
            break;

        default:
            break;
        }
        break;

    case engine::GUI::Event::Type::MouseEvent:
        switch(event.getMouseAction()) {
        case engine::GUI::Event::MouseAction::WheelDown:
            tryChangeSelection(false);
            break;

        case engine::GUI::Event::MouseAction::WheelUp:
            tryChangeSelection(true);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void HandyWindow_Inventory::draw()
{
    base::draw();

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &screenSize = device.getScreenSize();
    const auto &pos = screenSize - k_size;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();

    GUIRenderer.drawText("Inventory", pos, engine::Color::k_white);

    auto &thisPlayer = core.getThisPlayer();
    const auto &hands = thisPlayer.getCharacter().getInventory().getHandsItemContainer();

    int curY{pos.y + 29};

    for(size_t i = m_equippableItemsOffset; i < m_currentEquippableItems.size(); ++i) {
        if(static_cast <int> (i) - m_equippableItemsOffset > k_visibleEquippableItemsCount - 1)
            break;

        E_DASSERT(m_currentEquippableItems[i], "Item is nullptr.");

        engine::IntRect rect{{pos.x, curY - 8}, {k_size.x, 46}};

        if(hands.hasItem() && hands.getItemPtr() == m_currentEquippableItems[i])
            GUIRenderer.drawFilledRect(rect, {1.f, 1.f, 1.f, 0.13f});

        std::string label{std::to_string(i + 1) + ". " + m_currentEquippableItems[i]->getDef().getCapitalizedLabel()};

        GUIRenderer.drawText(label, {pos.x + 10, curY}, engine::Color::k_white);
        curY += 46;
    }

    if(m_equippableItemsOffset) {
        E_DASSERT(m_upArrowTexture, "Up arrow texture is nullptr.");
        m_upArrowTexture->draw({screenSize.x - m_upArrowTexture->getSize().x - 4, pos.y + 24});
    }

    if(m_equippableItemsOffset + k_visibleEquippableItemsCount < static_cast <int> (m_currentEquippableItems.size())) {
        E_DASSERT(m_downArrowTexture, "Down arrow texture is nullptr.");
        m_downArrowTexture->draw({screenSize.x - m_downArrowTexture->getSize().x - 4, screenSize.y - 55});
    }
}

HandyWindow::Type HandyWindow_Inventory::getType() const
{
    return Type::Inventory;
}

void HandyWindow_Inventory::tryChangeSelection(bool goUp)
{
    TRACK;

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();
    auto &inventory = thisPlayer.getCharacter().getInventory();
    auto &hands = inventory.getHandsItemContainer();
    auto &storage = inventory.getMultiSlotItemContainer();
    std::shared_ptr <Item> itemToEquip;

    // player can change selection only if idle
    if(hands.hasItem() && thisPlayer.getFPPItemModelController().getCurrentAction() != FPPItemModelController::CurrentAction::None)
        return;

    if(hands.hasItem()) {
        auto it = std::find(m_currentEquippableItems.begin(), m_currentEquippableItems.end(), hands.getItemPtr());

        if(it != m_currentEquippableItems.end()) {
            if(goUp) {
                if(it != m_currentEquippableItems.begin()) {
                    --it;
                    itemToEquip = *it;
                }
            }
            else {
                ++it;

                if(it != m_currentEquippableItems.end())
                    itemToEquip = *it;
            }
        }
        else if(!m_currentEquippableItems.empty())
            itemToEquip = m_currentEquippableItems[0];
    }
    else if(!m_currentEquippableItems.empty())
        itemToEquip = m_currentEquippableItems[0];

    if(itemToEquip) {
        if(hands.hasItem()) {
            if(hands.getItemPtr() == itemToEquip)
                return;

            int stack{hands.getItem().getStack()};
            int movedCount{storage.tryAddItem(hands.getItemPtr())};

            // if we added whole stack then we have to remove it from the previous container
            if(stack == movedCount)
                hands.tryRemoveItem(hands.getItem().getEntityID());
        }

        if(!hands.hasItem()) {
            int stack{itemToEquip->getStack()};
            int movedCount{hands.tryAddItem(itemToEquip)};

            // if we added whole stack then we have to remove it from the previous container
            if(stack == movedCount)
                storage.tryRemoveItem(itemToEquip->getEntityID());

            auto &defsCache = core.getDefsCache();
            auto &soundPool = core.getSoundPool();

            soundPool.play(defsCache.Sound_Select3);
        }
    }
}

void HandyWindow_Inventory::updateCurrentEquippableItems()
{
    m_currentEquippableItems.clear();

    const auto &thisPlayerInventory = Global::getCore().getThisPlayer().getCharacter().getInventory();
    const auto &hands = thisPlayerInventory.getHandsItemContainer();
    const auto &storage = thisPlayerInventory.getMultiSlotItemContainer();

    if(hands.hasItem())
        m_currentEquippableItems.push_back(hands.getItemPtr());

    for(const auto &elem : storage.getItems()) {
        E_DASSERT(elem.item, "Item is nullptr.");

        if(elem.item->getDef().getSlotType() == SingleSlotItemContainer::SlotType::Hands)
            m_currentEquippableItems.push_back(elem.item);
    }

    std::sort(m_currentEquippableItems.begin(), m_currentEquippableItems.end(), [](const auto &lhs, const auto &rhs) {
        return lhs->getEntityID() < rhs->getEntityID();
    });
}

void HandyWindow_Inventory::updateEquippableItemsOffset()
{
    const auto &hands = Global::getCore().getThisPlayer().getCharacter().getInventory().getHandsItemContainer();

    auto count = static_cast <int> (m_currentEquippableItems.size());
    m_equippableItemsOffset = engine::Math::clamp(m_equippableItemsOffset, 0, std::max(count - k_visibleEquippableItemsCount, 0));

    if(!hands.hasItem()) {
        m_equippableItemsOffset = 0;
        return;
    }

    const auto &it = std::find(m_currentEquippableItems.begin(), m_currentEquippableItems.end(), hands.getItemPtr());

    if(it != m_currentEquippableItems.end()) {
        int at{std::distance(m_currentEquippableItems.begin(), it)};

        if(at < m_equippableItemsOffset)
          m_equippableItemsOffset = at;

        if(at > m_equippableItemsOffset + k_visibleEquippableItemsCount - 1)
          m_equippableItemsOffset = at - k_visibleEquippableItemsCount + 1;
    }
    else
        m_equippableItemsOffset = 0;
}

const engine::IntVec2 HandyWindow_Inventory::k_size{300, 400};
const int HandyWindow_Inventory::k_visibleEquippableItemsCount{7};
const std::string HandyWindow_Inventory::k_upArrowTexturePath = "GUI/upArrow.png";
const std::string HandyWindow_Inventory::k_downArrowTexturePath = "GUI/downArrow.png";

} // namespace app
