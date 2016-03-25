#include "HandyWindow_Construct.hpp"

#include "../../entities/components/PlayerComponent.hpp"
#include "../../defs/StructureRecipeDef.hpp"
#include "../../defs/DefsCache.hpp"
#include "../../entities/Character.hpp"
#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../../SoundPool.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/Event.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/util/Color.hpp"

namespace app
{

HandyWindow_Construct::HandyWindow_Construct()
    : HandyWindow{k_size},
      m_structureDefCategory{StructureDef::Category::Wooden},
      m_unlockedStructuresOffset{}
{
    TRACK;

    m_availableCategories.push_back(StructureDef::Category::Wooden);
    m_availableCategories.push_back(StructureDef::Category::Stone);
    m_availableCategories.push_back(StructureDef::Category::Security);
    m_availableCategories.push_back(StructureDef::Category::Misc);

    auto &GUIRenderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    m_upArrowTexture = GUIRenderer.getTexture(k_upArrowTexturePath);
    m_downArrowTexture = GUIRenderer.getTexture(k_downArrowTexturePath);
}

void HandyWindow_Construct::update()
{
    TRACK;

    updateCurrentRecipes();
    updateCategoriesText();
    updateUnlockedStructuresOffset();
}

void HandyWindow_Construct::onEvent(engine::GUI::Event &event)
{
    TRACK;

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

        case irr::KEY_KEY_Q:
        case irr::KEY_LEFT:
            tryChangeCategory(false);
            break;

        case irr::KEY_KEY_E:
        case irr::KEY_RIGHT:
            tryChangeCategory(true);

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

void HandyWindow_Construct::draw()
{
    TRACK;

    base::draw();

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    auto &thisPlayer = core.getThisPlayer();
    const auto &screenSize = device.getScreenSize();
    const auto &pos = screenSize - k_size;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    const auto &structureCurrentlyDesignated = thisPlayer.getStructureCurrentlyDesignated();
    const auto &playerInventory = thisPlayer.getCharacter().getInventory();

    GUIRenderer.drawText(m_categoriesText, pos);

    int curY{pos.y + 29};

    for(size_t i = m_unlockedStructuresOffset; i < m_currentRecipes.size(); ++i) {
        if(static_cast <int> (i) - m_unlockedStructuresOffset > k_visibleRecipesCount - 1)
            break;

        E_DASSERT(m_currentRecipes[i], "Structure recipe def is nullptr.");

        const auto &structureDef = m_currentRecipes[i]->getStructureDef();
        engine::IntRect rect{{pos.x, curY - 8}, {k_size.x, 46}};

        if(structureCurrentlyDesignated.hasStructure() &&
           &structureCurrentlyDesignated.getStructureRecipeDef() == m_currentRecipes[i].get()) {
            GUIRenderer.drawFilledRect(rect, {1.f, 1.f, 1.f, 0.13f});
        }

        std::string label{std::to_string(i + 1) + ". " + structureDef.getCapitalizedLabel()};

        GUIRenderer.drawText(label, {pos.x + 10, curY}, engine::Color::k_white);
        curY += 13;

        GUIRenderer.drawText(m_currentRecipes[i]->getPrice().toRichText(playerInventory, ' '), {pos.x + 10, curY});
        curY += 33;

        if(!playerInventory.canAfford(m_currentRecipes[i]->getPrice()))
            GUIRenderer.drawFilledRect(rect, {1.f, 0.f, 0.f, 0.06f});
    }

    if(m_unlockedStructuresOffset) {
        E_DASSERT(m_upArrowTexture, "Up arrow texture is nullptr.");
        m_upArrowTexture->draw({screenSize.x - m_upArrowTexture->getSize().x - 4, pos.y + 24});
    }

    if(m_unlockedStructuresOffset + k_visibleRecipesCount < static_cast <int> (m_currentRecipes.size())) {
        E_DASSERT(m_downArrowTexture, "Down arrow texture is nullptr.");
        m_downArrowTexture->draw({screenSize.x - m_downArrowTexture->getSize().x - 4, screenSize.y - 55});
    }
}

HandyWindow::Type HandyWindow_Construct::getType() const
{
    return Type::Construct;
}

HandyWindow_Construct::~HandyWindow_Construct()
{
    TRACK;

    if(!Global::isNowDestroying())
        Global::getCore().getThisPlayer().getStructureCurrentlyDesignated().removeStructure();
}

void HandyWindow_Construct::tryChangeSelection(bool goUp)
{
    TRACK;

    auto &core = Global::getCore();
    auto &structureCurrentlyDesignated = core.getThisPlayer().getStructureCurrentlyDesignated();
    bool changed{};

    if(structureCurrentlyDesignated.hasStructure()) {
        auto it = std::find(m_currentRecipes.begin(), m_currentRecipes.end(), structureCurrentlyDesignated.getStructureRecipeDefPtr());

        if(it != m_currentRecipes.end()) {
            if(goUp) {
                if(it != m_currentRecipes.begin()) {
                    --it;
                    structureCurrentlyDesignated.setStructureRecipeDef(*it);
                    changed = true;
                }
            }
            else {
                ++it;

                if(it != m_currentRecipes.end()) {
                    structureCurrentlyDesignated.setStructureRecipeDef(*it);
                    changed = true;
                }
            }
        }
        else if(!m_currentRecipes.empty()) {
            structureCurrentlyDesignated.setStructureRecipeDef(m_currentRecipes[0]);
            changed = true;
        }
    }
    else if(!m_currentRecipes.empty()) {
        structureCurrentlyDesignated.setStructureRecipeDef(m_currentRecipes[0]);
        changed = true;
    }

    if(changed) {
        auto &defsCache = core.getDefsCache();
        auto &soundPool = core.getSoundPool();

        soundPool.play(defsCache.Sound_Select3);
    }
}

void HandyWindow_Construct::tryChangeCategory(bool goRight)
{
    TRACK;

    for(size_t i = 0; i < m_availableCategories.size(); ++i) {
        if(m_structureDefCategory == m_availableCategories[i]) {
            bool changed{};

            if(goRight && i < m_availableCategories.size() - 1) {
                m_structureDefCategory = m_availableCategories[i + 1];
                changed = true;
            }

            if(!goRight && i) {
                m_structureDefCategory = m_availableCategories[i - 1];
                changed = true;
            }

            if(changed) {
                auto &core = Global::getCore();
                auto &defsCache = core.getDefsCache();
                auto &soundPool = core.getSoundPool();

                soundPool.play(defsCache.Sound_Select3);
            }

            break;
        }
    }

    updateUnlockedStructuresOffset();
}

void HandyWindow_Construct::updateCurrentRecipes()
{
    const auto &unlockedRecipes = Global::getCore().getThisPlayer().getCharacter().getPlayerComponent().getUnlockedStructures().getUnlocked();

    m_currentRecipes.clear();

    for(const auto &elem : unlockedRecipes) {
        E_DASSERT(elem, "Structure recipe def is nullptr.");

        if(elem->getStructureDef().getCategory() == m_structureDefCategory)
            m_currentRecipes.push_back(elem);
    }

    std::sort(m_currentRecipes.begin(), m_currentRecipes.end(), [](const std::shared_ptr <StructureRecipeDef> &left, const std::shared_ptr <StructureRecipeDef> &right) {
        return left->getDefName() < right->getDefName();
    });
}

void HandyWindow_Construct::updateUnlockedStructuresOffset()
{
    const auto &structureCurrentlyDesignated = Global::getCore().getThisPlayer().getStructureCurrentlyDesignated();

    auto count = static_cast <int> (m_currentRecipes.size());
    m_unlockedStructuresOffset = engine::Math::clamp(m_unlockedStructuresOffset, 0, std::max(count - k_visibleRecipesCount, 0));

    if(!structureCurrentlyDesignated.hasStructure()) {
        m_unlockedStructuresOffset = 0;
        return;
    }

    const auto &currentlyDesignatedRecipe = structureCurrentlyDesignated.getStructureRecipeDefPtr();

    const auto &it = std::find(m_currentRecipes.begin(), m_currentRecipes.end(), currentlyDesignatedRecipe);

    if(it != m_currentRecipes.end()) {
        int at{std::distance(m_currentRecipes.begin(), it)};

        if(at < m_unlockedStructuresOffset)
          m_unlockedStructuresOffset = at;

        if(at > m_unlockedStructuresOffset + k_visibleRecipesCount - 1)
          m_unlockedStructuresOffset = at - k_visibleRecipesCount + 1;
    }
    else
        m_unlockedStructuresOffset = 0;
}

void HandyWindow_Construct::updateCategoriesText()
{
    engine::Color categoryColor{165, 177, 253};

    m_categoriesText.clear();

    for(const auto &category : m_availableCategories) {
        if(m_structureDefCategory == category)
            m_categoriesText << categoryColor;
        else
            m_categoriesText << categoryColor.changedAlpha(0.5f);

        m_categoriesText << category.toString() << "   ";
    }
}

const engine::IntVec2 HandyWindow_Construct::k_size{300, 400};
const int HandyWindow_Construct::k_visibleRecipesCount{7};
const std::string HandyWindow_Construct::k_upArrowTexturePath = "GUI/upArrow.png";
const std::string HandyWindow_Construct::k_downArrowTexturePath = "GUI/downArrow.png";

} // namespace app
