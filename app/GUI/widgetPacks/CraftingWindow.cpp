#include "CraftingWindow.hpp"

#include "../../world/World.hpp"
#include "../../itemContainers/MultiSlotItemContainer.hpp"
#include "../../entities/Character.hpp"
#include "../../entities/Structure.hpp"
#include "../../entities/Item.hpp"
#include "../../defs/DefsCache.hpp"
#include "../../defs/CraftingRecipeDef.hpp"
#include "../../defs/ItemDef.hpp"
#include "engine/GUI/widgets/Window.hpp"
#include "engine/GUI/widgets/Button.hpp"
#include "engine/GUI/widgets/RectWidget.hpp"
#include "engine/GUI/widgets/Image.hpp"
#include "engine/GUI/widgets/Label.hpp"
#include "../../Core.hpp"
#include "../../Global.hpp"
#include "../../SoundPool.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "../MainGUI.hpp"

namespace app
{

CraftingWindow::CraftingWindow(const std::shared_ptr <Character> &character)
    : m_character{character},
      m_rowsOffset{}
{
    if(!m_character)
        throw engine::Exception{"Character is nullptr."};

    init();
}

CraftingWindow::CraftingWindow(const std::shared_ptr <Character> &character, const std::shared_ptr <Structure> &workbench)
    : m_character{character},
      m_workbench{workbench},
      m_rowsOffset{}
{
    if(!m_character)
        throw engine::Exception{"Character is nullptr."};

    if(!m_workbench)
        throw engine::Exception{"Workbench is nullptr."};

    init();
}

void CraftingWindow::update()
{
}

bool CraftingWindow::isClosed() const
{
    E_DASSERT(m_window, "Window is nullptr.");
    return m_window->isDead();
}

bool CraftingWindow::hasWorkbench() const
{
    return static_cast <bool> (m_workbench);
}

Character &CraftingWindow::getCharacter() const
{
    E_DASSERT(m_character, "Character is nullptr.");
    return *m_character;
}

Structure &CraftingWindow::getWorkbench() const
{
    if(!m_workbench)
        throw engine::Exception{"Workbench structure is nullptr."};

    return *m_workbench;
}

const float CraftingWindow::k_maxCharacterDistanceToWorkbench{15.f};

void CraftingWindow::init()
{
    auto &device = Global::getCore().getDevice();
    auto &GUIManager = device.getGUIManager();
    const auto &screenSize = device.getScreenSize();

    m_window = GUIManager.addWindow({(screenSize - k_size) / 2, k_size});

    if(m_workbench)
        m_window->setTitle(m_workbench->getName());
    else
        m_window->setTitle(k_title);

    engine::IntVec2 nextPrevButtonSize{100, 40};

    m_pageLabel = m_window->addLabel({0, k_size.y - nextPrevButtonSize.y - 10, k_size.x, nextPrevButtonSize.y});
    m_pageLabel->setCentered(true, true);

    m_nextButton = m_window->addButton({k_size - nextPrevButtonSize.moved(10, 10), nextPrevButtonSize});
    m_nextButton->setText("Next");
    m_nextButton->setOnPressed([this]() {
        this->m_rowsOffset += k_rowsCountPerPage; // no bounds check, recreateRows should handle it
        this->recreateRows();
    });

    m_previousButton = m_window->addButton({{10, k_size.y - nextPrevButtonSize.y - 10}, nextPrevButtonSize});
    m_previousButton->setText("Previous");
    m_previousButton->setOnPressed([this]() {
        this->m_rowsOffset -= k_rowsCountPerPage; // no bounds check, recreateRows should handle it
        this->recreateRows();
    });

    recreateRows();
}

void CraftingWindow::recreateRows()
{
    auto &defsCache = Global::getCore().getDefsCache();

    auto craftingRecipes = defsCache.AllCraftingRecipes;

    for(size_t i = 0; i < craftingRecipes.size();) {
        bool isInvalidWorkbench{craftingRecipes[i]->requiresWorkbench() &&
                                (!m_workbench ||
                                 &m_workbench->getDef() != &craftingRecipes[i]->getRequiredWorkbenchStructureDef())};

        // if a recipe doesn't require a workbench, but we use one, then we don't want to show it
        if(!craftingRecipes[i]->requiresWorkbench() && m_workbench)
            isInvalidWorkbench = true;

        if(isInvalidWorkbench) {
            std::swap(craftingRecipes[i], craftingRecipes.back());
            craftingRecipes.pop_back();
        }
        else
            ++i;
    }

    std::sort(craftingRecipes.begin(), craftingRecipes.end(), [](const auto &lhs, const auto &rhs) {
        auto s1 = lhs->getSkillsRequirement().getArbitrarySeriousnessFactor();
        auto s2 = rhs->getSkillsRequirement().getArbitrarySeriousnessFactor();

        auto c1 = lhs->getPrice().getAllRequiredItemsTotalCount();
        auto c2 = rhs->getPrice().getAllRequiredItemsTotalCount();

        return std::tie(s1, c1, lhs->getDefName()) < std::tie(s2, c2, rhs->getDefName());
    });

    // omitted rows can be from 0 to size - 1 (so there's at least 1 recipe visible)
    m_rowsOffset = engine::Math::clamp(m_rowsOffset, 0, std::max(0, static_cast <int> (craftingRecipes.size()) - 1));

    // omitted rows count can only be a multiple of rows count per page
    m_rowsOffset -= m_rowsOffset % k_rowsCountPerPage;

    // if for some reason we omitted more than size recipes, then just set offset to 0
    if(m_rowsOffset >= static_cast <int> (craftingRecipes.size()))
        m_rowsOffset = 0;

    m_rows.clear();

    E_DASSERT(m_window, "Window is nullptr.");
    E_DASSERT(m_character, "Character is nullptr.");

    for(int i = m_rowsOffset, count = 0; i < static_cast <int> (craftingRecipes.size()); ++i, ++count) {
        if(count == k_rowsCountPerPage)
            break;

        Row newRow;

        newRow.rect = m_window->addRectWidget({1, 26 + count * 60, k_size.x - 1, 60});
        newRow.rect->setColor({0.f, 0.f, 0.f, count % 2 ? 0.35f : 0.25f});

        const auto &craftedItemDef = craftingRecipes[i]->getCraftedItemDef();

        newRow.itemLabel = m_window->addLabel({10, 26 + count * 60 + 2, 100, 30});
        newRow.itemLabel->setColor({0.8f, 0.8f, 0.8f});

        int craftedItemStack{craftingRecipes[i]->getCraftedItemStack()};

        if(craftedItemStack != 1)
            newRow.itemLabel->setText(std::to_string(craftedItemStack) + "x " + craftedItemDef.getCapitalizedLabel());
        else
            newRow.itemLabel->setText(craftedItemDef.getCapitalizedLabel());

        const engine::IntVec2 buttonSize{70, 30};

        newRow.craftButton = m_window->addButton({{k_size.x - 10 - buttonSize.x, 26 + count * 60 + 15}, buttonSize});
        newRow.craftButton->setText("Craft");
        newRow.craftButton->setOnPressed([this, recipe = craftingRecipes[i]]() {
            E_DASSERT(this->m_character, "Character is nullptr.");

            if(this->m_character->getInventory().canAfford(recipe->getPrice()) &&
               recipe->getSkillsRequirement().isMet(*this->m_character)) {
                auto item = std::make_shared <Item> (Global::getCore().getWorld().getUniqueEntityID(), recipe->getCraftedItemDefPtr(), recipe->getCraftedItemStack());

                // if added at least 1 item from the produced stack (there could be no free space in inventory)
                if(this->m_character->getInventory().getMultiSlotItemContainer().tryAddItem(item)) {
                    this->m_character->getInventory().pay(recipe->getPrice());

                    auto &core = Global::getCore();
                    auto &defsCache = core.getDefsCache();
                    auto &soundPool = core.getSoundPool();

                    soundPool.play(defsCache.Sound_Crafted);

                    this->recreateRows();
                }
            }
        });

        const auto &texturePtr = craftedItemDef.getTextureInInventoryPtr();

        const auto &imageRect = engine::IntRect{{}, texturePtr->getSize()}.fittedToSquareWithAspect({10, 26 + count * 60}, 60);

        newRow.itemImage = m_window->addImage(imageRect);
        newRow.itemImage->setTexture(texturePtr);
        newRow.itemImage->setOnMouseOver([itemDefPtr = craftingRecipes[i]->getCraftedItemDefPtr()]() {
            Global::getCore().getMainGUI().getTooltip().setItemDef(itemDefPtr);
        });

        newRow.priceLabel = m_window->addLabel({120, 26 + count * 60 + 2, 100, 58});
        newRow.priceLabel->setText(craftingRecipes[i]->getPrice().toRichText(m_character->getInventory(), '\n'));

        newRow.skillsRequirementLabel = m_window->addLabel({235, 26 + count * 60 + 2, 100, 58});
        newRow.skillsRequirementLabel->setText(craftingRecipes[i]->getSkillsRequirement().toRichText(*m_character));

        m_rows.emplace_back(std::move(newRow));
    }

    E_DASSERT(m_pageLabel, "Label is nullptr.");

    m_pageLabel->setText("Page " + std::to_string(m_rowsOffset / k_rowsCountPerPage + 1) + '/' + std::to_string(craftingRecipes.size() / k_rowsCountPerPage + 1));
}

const engine::IntVec2 CraftingWindow::k_size{500, 500};
const std::string CraftingWindow::k_title = "Crafting";
const int CraftingWindow::k_rowsCountPerPage{7};

} // namespace app
